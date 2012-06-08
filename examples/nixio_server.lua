
local epoller = require"epoller"
local nixio = require"nixio"

local poller = epoller.new()

local wsock_meth = {}
local wsock_mt = { __index = wsock_meth }

function wsock_meth:fileno()
	return self.sock:fileno()
end

function wsock_meth:close()
	local sock = self.sock
	if not sock then return end
	local fd = sock:fileno()
	poller:del(self)
	self.sock = nil
	sock:close()
end

local function wrap_sock(sock, on_io_event)
	return setmetatable({ sock = sock, on_io_event = on_io_event }, wsock_mt)
end

local function accept_connection(server, cb)
	local client = server:accept()
	local fd = client:fileno()
	client:setblocking(false)
	client = wrap_sock(client, cb)
	-- register callback for read events.
	poller:add(client, epoller.EPOLLIN)
	return client
end

local function new_acceptor(host, port, family, cb)
	local sock = nixio.socket(family or 'inet', 'stream')
	local fd = sock:fileno()
	sock:setblocking(false)
	assert(sock:setsockopt('socket', 'reuseaddr', 1))
	if host == '*' then host = nil end
	assert(sock:bind(host, port))
	assert(sock:listen(1024))
	-- register callback for read events.
	sock = wrap_sock(sock, cb)
	poller:add(sock, epoller.EPOLLIN)
	return sock
end

local function new_client(server)
	accept_connection(server, function(wsock, events)
		local sock = wsock.sock
		local msg = sock:recv(1024)
		if msg and #msg > 0 then
			sock:send("echo:" .. msg)
		else
			print('closing client')
			wsock:close()
		end
	end)
end

local function new_server(port)
	print("listen on:", port)
	new_acceptor('*', port, 'inet', function(wsock, events)
		print("accept new client on:", port)
		new_client(wsock.sock)
	end)
end

for i=1,#arg do
	new_server(arg[i])
end

if #arg == 0 then
	new_server("1080")
end

poller:start()

