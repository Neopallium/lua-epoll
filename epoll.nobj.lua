
-- make generated variable nicer.
set_variable_format "%s"

c_module "epoll" {

-- enable FFI bindings support.
luajit_ffi = true,
luajit_ffi_load_cmodule = true,

sys_include "sys/epoll.h",
sys_include "unistd.h",

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
"src/error.nobj.lua",
"src/epoller.nobj.lua",
},

c_function "new" {
	var_in{ "int", "size", is_optional = true, default = 64 },
	c_export_call { "Epoller *", "!this" } "epoller_create" { "int", "size"},
},
}

