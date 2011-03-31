-- Copyright (c) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

local epoll_types = [[
typedef union epoll_data {
	void        *ptr;
	int          fd;
	uint32_t     u32;
	uint64_t     u64;
} epoll_data_t;

struct epoll_event {
	uint32_t     events;      /* Epoll events */
	epoll_data_t data;        /* User data variable */
} __attribute__ ((__packed__));

typedef struct epoll_event epoll_event;
]]

local Epoller_type = [[
typedef struct Epoller {
	int epfd;
	int size;
	int count;
	struct epoll_event *events;
} Epoller;
]]

object "Epoller" {
	sys_include "string.h",
	-- main Epoller C code.
	c_source(Epoller_type),
	c_source[[
#define EPOLLER_MIN_SIZE 20
static void epoller_resize(Epoller *this, int newsize) {
	if(newsize < EPOLLER_MIN_SIZE) newsize = EPOLLER_MIN_SIZE;
	this->events = (struct epoll_event *)realloc(this->events, newsize * sizeof(struct epoll_event));
	this->size = newsize;
}

static void epoller_grow(Epoller *this) {
	if(this->size > this->count) return;
	epoller_resize(this, this->count + 50);
}

static Epoller *epoller_create(int size) {
	Epoller *this;

	this = (Epoller *)calloc(1, sizeof(Epoller));
#ifdef EPOLL_CLOEXEC
	this->epfd = epoll_create1(EPOLL_CLOEXEC);
#else
	this->epfd = epoll_create(1024);
#endif
	this->size = 0;
	this->count = 0;
	this->events = NULL;
	epoller_resize(this, size);

	return this;
}

static void epoller_destroy(Epoller *this) {
	close(this->epfd);
	free(this->events);
	free(this);
}

static int epoller_ctl(Epoller *this, int op, int fd, uint32_t events, uint64_t id) {
	struct epoll_event event;
	event.events = events;
	event.data.u64 = id;
	return epoll_ctl(this->epfd, op, fd, &event);
}

static int epoller_add(Epoller *this, int fd, uint32_t events, uint64_t id) {
	this->count++;
	epoller_grow(this);
	return epoller_ctl(this, EPOLL_CTL_ADD, fd, events, id);
}

static int epoller_mod(Epoller *this, int fd, uint32_t events, uint64_t id) {
	return epoller_ctl(this, EPOLL_CTL_MOD, fd, events, id);
}

static int epoller_del(Epoller *this, int fd) {
	this->count--;
	return epoller_ctl(this, EPOLL_CTL_DEL, fd, 0, 0);
}

static int epoller_wait(Epoller *this, int timeout) {
	return epoll_wait(this->epfd, this->events, this->size, timeout);
}

]],
	-- register epoll & Epoller datastures with FFI.
	ffi_cdef(epoll_types),
	ffi_cdef(Epoller_type),
	-- export epoller C interface to FFI code.
	ffi_export_function "int" "epoller_add" "(Epoller *this, int fd, uint32_t events, uint64_t id)",
	ffi_export_function "int" "epoller_mod" "(Epoller *this, int fd, uint32_t events, uint64_t id)",
	ffi_export_function "int" "epoller_del" "(Epoller *this, int fd)",
	ffi_export_function "int" "epoller_wait" "(Epoller *this, int timeout)",
  constructor {
		var_in{ "int", "size", is_optional = true, default = 64 },
		c_source[[
	${this} = epoller_create(${size});
]],
  },
  destructor {
		c_source[[
	epoller_destroy(${this});
]],
  },

  method "add" {
		var_in{ "int", "fd" },
		var_in{ "uint32_t", "events" },
		var_in{ "uint64_t", "id" },
		var_out{ "int", "rc" },
		c_source[[
	${rc} = epoller_add(${this}, ${fd}, ${events}, ${id});
]],
		ffi_source[[
	${rc} = epoller_add(${this}, ${fd}, ${events}, ${id});
]],
  },
  method "mod" {
		var_in{ "int", "fd" },
		var_in{ "uint32_t", "events" },
		var_in{ "uint64_t", "id" },
		var_out{ "int", "rc" },
		c_source[[
	${rc} = epoller_mod(${this}, ${fd}, ${events}, ${id});
]],
		ffi_source[[
	${rc} = epoller_mod(${this}, ${fd}, ${events}, ${id});
]],
  },
  method "del" {
		var_in{ "int", "fd" },
		var_out{ "int", "rc" },
		c_source[[
	${rc} = epoller_del(${this}, ${fd});
]],
		ffi_source[[
	${rc} = epoller_del(${this}, ${fd});
]],
  },
  method "wait" {
		var_in{ "<any>", "events" },
		var_in{ "int", "timeout" },
		var_out{ "int", "rc" },
		c_source[[
	luaL_checktype(L, ${events::idx}, LUA_TTABLE);
	${rc} = epoller_wait(${this}, ${timeout});
	if(${rc} > 0) {
		int idx;
		int n;
		/* fill 'events' table with event <id, events> pairs. */
		for(n = 0, idx = 1; n < ${rc}; n++) {
			lua_pushinteger(L, ${this}->events[n].data.u64);
			lua_rawseti(L, ${events::idx}, idx); idx++;
			lua_pushinteger(L, ${this}->events[n].events);
			lua_rawseti(L, ${events::idx}, idx); idx++;
		}
	}
]],
 		ffi_source[[
	${rc} = epoller_wait(${this}, ${timeout});
	if (${rc} > 0) then
		local idx = 1
		-- fill 'events' table with event <id, events> pairs.
		for n=0,(${rc}-1) do
			${events}[idx] = tonumber(${this}.events[n].data.u64)
			idx = idx + 1
			${events}[idx] = tonumber(${this}.events[n].events)
			idx = idx + 1
		end
	end
]],
 },
}
