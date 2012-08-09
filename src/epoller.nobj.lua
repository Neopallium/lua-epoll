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

int epoller_wait(Epoller *this, int timeout);
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

Epoller *epoller_create(int size) {
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

void epoller_destroy(Epoller *this) {
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

int epoller_add(Epoller *this, int fd, uint32_t events, uint64_t id) {
	this->count++;
	epoller_grow(this);
	return epoller_ctl(this, EPOLL_CTL_ADD, fd, events, id);
}

int epoller_mod(Epoller *this, int fd, uint32_t events, uint64_t id) {
	return epoller_ctl(this, EPOLL_CTL_MOD, fd, events, id);
}

int epoller_del(Epoller *this, int fd) {
	this->count--;
	return epoller_ctl(this, EPOLL_CTL_DEL, fd, 0, 0);
}

int epoller_wait(Epoller *this, int timeout) {
	return epoll_wait(this->epfd, this->events, this->size, timeout);
}

]],
	-- register epoll & Epoller datastures with FFI.
	ffi_cdef(epoll_types),
	ffi_cdef(Epoller_type),

  constructor {
		var_in{ "int", "size", is_optional = true, default = 64 },
		c_call "Epoller *" "epoller_create" { "int", "size"},
  },
  destructor "close" {
		c_method_call "void" "epoller_destroy" {},
  },

  method "add" {
		c_method_call "errno_rc" "epoller_add" { "int", "fd", "uint32_t", "events", "uint64_t", "id" },
  },
  method "mod" {
		c_method_call "errno_rc" "epoller_mod" { "int", "fd", "uint32_t", "events", "uint64_t", "id" },
  },
  method "del" {
		c_method_call "errno_rc" "epoller_del" { "int", "fd" },
  },
  method "wait" {
		var_in{ "<any>", "events" },
		c_source "pre_src" [[
	luaL_checktype(L, ${events::idx}, LUA_TTABLE);
]],
		c_method_call { "errno_rc", "rc" } "epoller_wait" { "int", "timeout" },
		c_source[[
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
  method "wait_callback" {
		var_in{ "<any>", "event_cb" },
		c_source "pre_src" [[
	luaL_checktype(L, ${event_cb::idx}, LUA_TFUNCTION);
]],
		c_method_call { "errno_rc", "rc" } "epoller_wait" { "int", "timeout" },
		c_source[[
	if(${rc} > 0) {
		int n;
		/* call 'event_cb' for each <id, events> pair. */
		for(n = 0; n < ${rc}; n++) {
			lua_pushvalue(L,  ${event_cb::idx});
			lua_pushinteger(L, ${this}->events[n].data.u64);
			lua_pushinteger(L, ${this}->events[n].events);
			lua_call(L, 2, 0);
		}
		lua_pushinteger(L, rc);
		return 1;
	} else if(${rc} == 0) {
		lua_pushinteger(L, rc);
		return 1;
	}
]],
 		ffi_source[[
	if (${rc} > 0) then
		-- call 'event_cb' for each <id, events> pair.
		for n=0,(${rc}-1) do
			${event_cb}(tonumber(${this}.events[n].data.u64), tonumber(${this}.events[n].events))
		end
		return ${rc}
	elseif (${rc} == 0) then
		return ${rc}
	end
]],
 },
}
