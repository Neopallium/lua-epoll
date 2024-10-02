#
# Lua bindings for epoll
#
cmake_minimum_required(VERSION 3.18)

project(lua-epoll C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_CMOD share/lua/cmod CACHE PATH "Directory to install Lua binary modules (configure lua via LUA_CPATH)")

set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

## Lua 5.x
include(FindLua)
if(NOT ${LUA_FOUND})
    message(FATAL_ERROR "The FindLua module could not find lua :-(")
endif()
set(COMMON_LIBS "${COMMON_LIBS};${LUA_LIBRARIES}")

if(WIN32)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -I${LUA_INCLUDE_DIR}")
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${LUA_LIBRARY}")
endif()
## MAC OSX needs extra linker flags
if(APPLE)
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -undefined dynamic_lookup")
endif()

## check for epoll support
include(CheckCSourceCompiles)
check_c_source_compiles("
#include <sys/epoll.h>

int main(int argc, char *argv[]) {
	int fd = epoll_create(10);
	return 0;
}" HAVE_EPOLL)

if(NOT HAVE_EPOLL)
	message(FATAL_ERROR "epoll support required!")
endif(NOT HAVE_EPOLL)

## LuaNativeObjects
include(LuaNativeObjects)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${LUA_INCLUDE_DIR})

## LuaEpoll
set(LUA_EPOLL_SRC
	epoll.nobj.lua
)

if(${USE_PRE_GENERATED_BINDINGS})
	set(LUA_EPOLL_SRC src/pre_generated-epoll.nobj.c)
else()
	# Generate Lua bindings.
	GenLuaNativeObjects(LUA_EPOLL_SRC)
endif()

add_library(lua-epoll MODULE ${LUA_EPOLL_SRC})
target_link_libraries(lua-epoll ${COMMON_LIBS})
set_target_properties(lua-epoll PROPERTIES PREFIX "")
set_target_properties(lua-epoll PROPERTIES COMPILE_FLAGS "${COMMON_CFLAGS}")
set_target_properties(lua-epoll PROPERTIES OUTPUT_NAME epoll)

install(TARGETS lua-epoll
        DESTINATION "${INSTALL_CMOD}")

