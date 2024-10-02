lua-epoll
=======

[![travis-ci status](https://secure.travis-ci.org/Neopallium/lua-epoll.png?branch=master)](http://travis-ci.org/Neopallium/lua-epoll/builds)

Lightweight epoll wrapper.

See the `examples/nixio_server.lua` for an example of how to use.

Installing
----------

### Install lua-epoll version 1.0:

	luarocks install Neopallium/lua-epoll 1.0-1


### Install lua-epoll:

	luarocks install https://github.com/Neopallium/lua-epoll/raw/master/lua-epoll-scm-0.rockspec


To re-generating the bindings
-----------------------------

You will need to install LuaNativeObjects and set the CMake variable `USE_PRE_GENERATED_BINDINGS` to FALSE.
By default CMake will use the pre-generated bindings that are include in the project.

Build Dependencies
------------------

Optional dependency for re-generating Lua bindings from `*.nobj.lua` files:

* [LuaNativeObjects](https://github.com/Neopallium/LuaNativeObjects), this is the bindings generator used to convert the `*.nobj.lua` files into a native Lua module.

