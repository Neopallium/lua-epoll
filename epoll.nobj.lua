
-- make generated variable nicer.
set_variable_format "%s"

c_module "epoll" {

-- enable FFI bindings support.
luajit_ffi = true,

sys_include "sys/epoll.h",

export_definitions {
"EPOLLIN",
"EPOLLOUT",
"EPOLLRDHUP",
"EPOLLPRI",
"EPOLLERR",
"EPOLLET",
"EPOLLONESHOT",
},

subfiles {
"src/epoller.nobj.lua",
},

c_function "new" {
	var_in{ "int", "size", is_optional = true, default = 64 },
	var_out{ "!Epoller *", "this" },
	c_source[[
	${this} = epoller_create(${size});
]],
},
}

