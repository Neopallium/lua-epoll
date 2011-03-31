#!/usr/bin/env lua

package	= 'lua-epoll'
version	= 'scm-0'
source	= {
	url	= 'git://github.com/Neopallium/lua-epoll.git'
}
description	= {
	summary	= "Lightweight wrapper for epoll.",
	detailed	= '',
	homepage	= 'https://github.com/Neopallium/lua-epoll',
	license	= 'MIT',
	maintainer = "Robert G. Jakabosky",
}
dependencies = {
	'lua >= 5.1',
}
build	= {
	type = "builtin",
	modules = {
		epoll = {
			sources = { "src/pre_generated-epoll.nobj.c" },
		}
	}
}
