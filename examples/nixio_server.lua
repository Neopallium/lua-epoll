
local epoll = require"epoll"
local nixio = require"nixio"

local epoller = epoll.new()

local socks = {}
local cbs = {}

local function poll_add(sock, events, cb)
	local fd = sock:fileno()
	cbs[fd] = cb
	return epoller:add(fd, events, fd)
end

local function poll_mod(sock, events, cb)
	local fd = sock:fileno()
	cbs[fd] = cb
	return epoller:mod(fd, events, fd)
end

local function poll_del(sock)
	local fd = sock:fileno()
	cbs[fd] = nil
	return epoller:del(fd)
end

local function poll_loop()
	local events = {}
	while true do
		assert(epoller:wait(events, -1))
		for i=1,#events,2 do
			local fd = events[i]
			local ev = events[i]
			local sock = socks[fd]
			local cb = cbs[fd]
			if sock and cb then
				cb(sock, ev)
			end
		end
	end
end

--[[
local function new_connection(host, port, family, stype)
	local sock = nixio.connect(host, port, family or 'any', stype or 'stream')
	local fd = sock:fileno()
	socks[fd] = sock
	sock:setblocking(false)
	return sock
end
]]

local function accept_connection(sock, cb)
	local client = sock:accept()
	local fd = client:fileno()
	socks[fd] = client
	client:setblocking(false)
	-- register callback for read events.
	poll_add(client, epoll.EPOLLIN, cb)
	return client
end

local function new_acceptor(host, port, family, cb)
	local sock = nixio.socket(family or 'inet', 'stream')
	local fd = sock:fileno()
	socks[fd] = sock
	sock:setblocking(false)
	assert(sock:setsockopt('socket', 'reuseaddr', 1))
	if host == '*' then host = nil end
	assert(sock:bind(host, port))
	assert(sock:listen(1024))
	-- register callback for read events.
	poll_add(sock, epoll.EPOLLIN, cb)
	return sock
end

local function sock_close(sock)
	local fd = sock:fileno()
	socks[fd] = nil
	poll_del(sock)
	sock:close()
end

local function new_client(server)
	accept_connection(server, function(sock, events)
		print(sock, events)
		local msg = sock:recv(1024)
		if msg and #msg > 0 then
			sock:send("echo:" .. msg)
		else
			print('closing client')
			sock_close(sock)
		end
	end)
end

local function new_server(host_port)
	local host, port = host_port:match("([*.0-9]*):([0-9]*)")
	print("listen on:", host, port)
	new_acceptor(host, port, 'inet', function(sock, events)
		print("accept new client on:", host, port)
		new_client(sock)
	end)
end

for i=1,#arg do
	new_server(arg[i])
end

poll_loop()

