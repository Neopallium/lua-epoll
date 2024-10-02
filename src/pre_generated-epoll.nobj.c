/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/*
** Adapted from Lua 5.2.0 luaL_setfuncs.
*/
static void nobj_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM >= 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 1


#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif



typedef int errno_rc;

static void error_code__errno_rc__push(lua_State *L, errno_rc err);


static obj_type obj_types[] = {
#define obj_type_id_Errors 0
#define obj_type_Errors (obj_types[obj_type_id_Errors])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Errors" },
#define obj_type_id_Epoller 1
#define obj_type_Epoller (obj_types[obj_type_id_Epoller])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "Epoller" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua 5.2 don't register modules as globals. */
#if LUA_VERSION_NUM == 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	nobj_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		nobj_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	nobj_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}



static char *obj_interfaces[] = {
  NULL,
};



#define obj_type_Epoller_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Epoller))
#define obj_type_Epoller_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Epoller))
#define obj_type_Epoller_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Epoller), flags)
#define obj_type_Epoller_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Epoller), flags)




static const char *epoll_ffi_lua_code[] = { "local ffi=require\"ffi\"\n"
"local function ffi_safe_load(name, global)\n"
"	local stat, C = pcall(ffi.load, name, global)\n"
"	if not stat then return nil, C end\n"
"	if global then return ffi.C end\n"
"	return C\n"
"end\n"
"local function ffi_load(name, global)\n"
"	return assert(ffi_safe_load(name, global))\n"
"end\n"
"\n"
"local ffi_string = ffi.string\n"
"\n"
"local f_cast = ffi.cast\n"
"local pcall = pcall\n"
"local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local sformat = require\"string\".format\n"
"local rawset = rawset\n"
"local setmetatable = setmetatable\n"
"local package = (require\"package\") or {}\n"
"local p_config = package.config\n"
"local p_cpath = package.cpath\n"
"\n"
"\n"
"local ffi_load_cmodule\n"
"\n"
"-- try to detect luvit.\n"
"if p_config == nil and p_cpath == nil then\n"
"	ffi_load_cmodule = function(name, global)\n"
"		for path,module in pairs(package.loaded) do\n"
"			if module == name then\n"
"				local C, err = ffi_safe_load(path, global)\n"
"				-- return opened library\n"
"				if C then return C end\n"
"			end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"else\n"
"	ffi_load_cmodule = function(name, global)\n"
"		local dir_sep = p_config:sub(1,1)\n"
"		local path_sep = p_config:sub(3,3)\n"
"		local path_mark = p_config:sub(5,5)\n"
"		local path_match = \"([^\" .. path_sep .. \"]+)\"\n"
"		-- convert dotted name to directory path.\n"
"		name = name:gsub('%.', dir_sep)\n"
"		-- try each path in search path.\n"
"		for path in p_cpath:gmatch(path_match) do\n"
"			local fname = path:gsub(path_mark, name)\n"
"			local C, err = ffi_safe_load(fname, global)\n"
"			-- return opened library\n"
"			if C then return C end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"end\n"
"\n"
"local _M, _priv, reg_table = ...\n"
"local REG_OBJECTS_AS_GLOBALS = false\n"
"local C = ffi.C\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"\n"
"local function ffi_safe_cdef(block_name, cdefs)\n"
"	local fake_type = \"struct sentinel_\" .. block_name .. \"_ty\"\n"
"	local stat, size = pcall(ffi.sizeof, fake_type)\n"
"	if stat and size > 0 then\n"
"		-- already loaded this cdef block\n"
"		return\n"
"	end\n"
"	cdefs = fake_type .. \"{ int a; int b; int c; };\" .. cdefs\n"
"	return ffi.cdef(cdefs)\n"
"end\n"
"\n"
"ffi_safe_cdef(\"LuaNativeObjects\", [[\n"
"\n"
"typedef struct obj_type obj_type;\n"
"\n"
"typedef void (*base_caster_t)(void **obj);\n"
"\n"
"typedef void (*dyn_caster_t)(void **obj, obj_type **type);\n"
"\n"
"struct obj_type {\n"
"	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */\n"
"	int32_t         id;       /**< type's id. */\n"
"	uint32_t        flags;    /**< type's flags (weak refs) */\n"
"	const char      *name;    /**< type's object name. */\n"
"};\n"
"\n"
"typedef struct obj_base {\n"
"	int32_t        id;\n"
"	base_caster_t  bcaster;\n"
"} obj_base;\n"
"\n"
"typedef struct obj_udata {\n"
"	void     *obj;\n"
"	uint32_t flags;  /**< lua_own:1bit */\n"
"} obj_udata;\n"
"\n"
"int memcmp(const void *s1, const void *s2, size_t n);\n"
"\n"
"]])\n"
"\n"
"local nobj_callback_states = {}\n"
"local nobj_weak_objects = setmetatable({}, {__mode = \"v\"})\n"
"local nobj_obj_flags = {}\n"
"\n"
"local function obj_ptr_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', ptr))\n"
"end\n"
"\n"
"local function obj_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', f_cast('void *', ptr)))\n"
"end\n"
"\n"
"local function register_default_constructor(_pub, obj_name, constructor)\n"
"	local obj_pub = _pub[obj_name]\n"
"	if type(obj_pub) == 'table' then\n"
"		-- copy table since it might have a locked metatable\n"
"		local new_pub = {}\n"
"		for k,v in pairs(obj_pub) do\n"
"			new_pub[k] = v\n"
"		end\n"
"		setmetatable(new_pub, { __call = function(t,...)\n"
"			return constructor(...)\n"
"		end,\n"
"		__metatable = false,\n"
"		})\n"
"		obj_pub = new_pub\n"
"	else\n"
"		obj_pub = constructor\n"
"	end\n"
"	_pub[obj_name] = obj_pub\n"
"	_M[obj_name] = obj_pub\n"
"	if REG_OBJECTS_AS_GLOBALS then\n"
"		_G[obj_name] = obj_pub\n"
"	end\n"
"end\n"
"\n"
"ffi_safe_cdef(\"BufferIF\", [[\n"
"typedef struct Buffer_if {\n"
"  const uint8_t * (* const const_data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} BufferIF;\n"
"]])\n"
"\n"
"ffi_safe_cdef(\"MutableBufferIF\", [[\n"
"typedef struct MutableBuffer_if {\n"
"  uint8_t * (* const data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} MutableBufferIF;\n"
"]])\n"
"\n"
"ffi_safe_cdef(\"FDIF\", [[\n"
"typedef struct FD_if {\n"
"  int (* const get_fd)(void *this_v);\n"
"  int (* const get_type)(void *this_v);\n"
"} FDIF;\n"
"]])\n"
"\n"
"local Cmod = ffi_load_cmodule(\"epoll\", false)\n"
"local C = Cmod\n"
"\n"
"ffi.cdef[[\n"
"typedef int errno_rc;\n"
"\n"
"typedef struct Epoller Epoller;\n"
"\n"
"]]\n"
"\n"
"ffi.cdef[[\n"
"typedef union epoll_data {\n"
"	void        *ptr;\n"
"	int          fd;\n"
"	uint32_t     u32;\n"
"	uint64_t     u64;\n"
"} epoll_data_t;\n"
"\n"
"struct epoll_event {\n"
"	uint32_t     events;      /* Epoll events */\n"
"	epoll_data_t data;        /* User data variable */\n"
"} __attribute__ ((__packed__));\n"
"\n"
"typedef struct epoll_event epoll_event;\n"
"\n"
"typedef struct Epoller {\n"
"	int epfd;\n"
"	int size;\n"
"	int count;\n"
"	struct epoll_event *events;\n"
"} Epoller;\n"
"\n"
"int epoller_wait(Epoller *this, int timeout);\n"
"\n"
"Epoller * epoller_create(int);\n"
"\n"
"void epoller_destroy(Epoller *);\n"
"\n"
"errno_rc epoller_add(Epoller *, int, uint32_t, uint64_t);\n"
"\n"
"errno_rc epoller_mod(Epoller *, int, uint32_t, uint64_t);\n"
"\n"
"errno_rc epoller_del(Epoller *, int);\n"
"\n"
"errno_rc epoller_wait(Epoller *, int);\n"
"\n"
"\n"
"]]\n"
"\n"
"REG_OBJECTS_AS_GLOBALS = false\n"
"local _obj_interfaces_ffi = {}\n"
"local _pub = {}\n"
"local _meth = {}\n"
"local _push = {}\n"
"local _obj_subs = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' then\n"
"		_obj_subs[obj_name] = {}\n"
"		if mt.__index then\n"
"			_meth[obj_name] = mt.__index\n"
"		end\n"
"	end\n"
"end\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"--\n"
"-- CData Metatable access\n"
"--\n"
"local _ctypes = {}\n"
"local _type_names = {}\n"
"local _get_mt_key = {}\n"
"local _ctype_meta_map = {}\n"
"\n"
"local f_typeof = ffi.typeof\n"
"local function get_cdata_type_id(cdata)\n"
"	return tonumber(f_typeof(cdata))\n"
"end\n"
"local function get_cdata_mt(cdata)\n"
"	return _ctype_meta_map[tonumber(f_typeof(cdata))]\n"
"end\n"
"\n"
"local function obj_register_ctype(name, ctype)\n"
"	local obj_mt = _priv[name]\n"
"	local obj_type = obj_mt['.type']\n"
"	local obj_ctype = ffi.typeof(ctype)\n"
"	local obj_type_id = tonumber(obj_ctype)\n"
"	_ctypes[name] = obj_ctype\n"
"	_type_names[name] = tostring(obj_ctype)\n"
"	_ctype_meta_map[obj_type_id] = obj_mt\n"
"	_ctype_meta_map[obj_mt] = obj_type_id\n"
"	return obj_mt, obj_type, obj_ctype\n"
"end\n"
"\n"
"--\n"
"-- Interfaces helper code.\n"
"--\n"
"local _obj_interfaces_key = \"obj_interfaces<1.0>_table_key\"\n"
"local _obj_interfaces_ud = reg_table[_obj_interfaces_key]\n"
"local _obj_interfaces_key_ffi = _obj_interfaces_key .. \"_LJ2_FFI\"\n"
"_obj_interfaces_ffi = reg_table[_obj_interfaces_key_ffi]\n"
"if not _obj_interfaces_ffi then\n"
"	-- create missing interfaces table for FFI bindings.\n"
"	_obj_interfaces_ffi = {}\n"
"	reg_table[_obj_interfaces_key_ffi] = _obj_interfaces_ffi\n"
"end\n"
"\n"
"local function obj_get_userdata_interface(if_name, expected_err)\n"
"	local impls_ud = _obj_interfaces_ud[if_name]\n"
"	if not impls_ud then\n"
"		impls_ud = {}\n"
"		_obj_interfaces_ud[if_name] = impls_ud\n"
"	end\n"
"	-- create cdata check function to be used by non-ffi bindings.\n"
"	if not impls_ud.cdata then\n"
"		function impls_ud.cdata(obj)\n"
"			return assert(impls_ud[get_cdata_mt(obj)], expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ud\n"
"end\n"
"\n"
"local function obj_get_interface_check(if_name, expected_err)\n"
"	local impls_ffi = _obj_interfaces_ffi[if_name]\n"
"	if not impls_ffi then\n"
"		local if_type = ffi.typeof(if_name .. \" *\")\n"
"		local impls_ud = obj_get_userdata_interface(if_name, expected_err)\n"
"		-- create table for FFI-based interface implementations.\n"
"		impls_ffi = setmetatable({}, {\n"
"		__index = function(impls_ffi, mt)\n"
"			local impl = impls_ud[mt]\n"
"			if impl then\n"
"				-- cast to cdata\n"
"				impl = if_type(impl)\n"
"				rawset(impls_ffi, mt, impl)\n"
"			end\n"
"			return impl\n"
"		end})\n"
"		_obj_interfaces_ffi[if_name] = impls_ffi\n"
"\n"
"		-- create check function for this interface.\n"
"		function impls_ffi.check(obj)\n"
"			local impl\n"
"			if type(obj) == 'cdata' then\n"
"				impl = impls_ffi[get_cdata_type_id(obj)]\n"
"			else\n"
"				impl = impls_ud.userdata(impls_ffi, obj)\n"
"			end\n"
"			return assert(impl, expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ffi.check\n"
"end\n"
"\n"
"local function obj_register_interface(if_name, obj_name)\n"
"	-- loopkup cdata id\n"
"	local obj_mt = _priv[obj_name]\n"
"	local obj_type_id = _ctype_meta_map[obj_mt]\n"
"	local impl_meths = {}\n"
"	local ffi_impls = _obj_interfaces_ffi[if_name]\n"
"	ffi_impls[obj_type_id] = impl_meths\n"
"	_meth[obj_name]['NOBJ_get_' .. if_name] = impl_meths\n"
"	return impl_meths\n"
"end\n"
"\n"
"\n"
"local obj_type_Epoller_check\n"
"local obj_type_Epoller_delete\n"
"local obj_type_Epoller_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"Epoller\", \"Epoller *\")\n"
"\n"
"	function obj_type_Epoller_check(ptr)\n"
"		-- if ptr is nil or is the correct type, then just return it.\n"
"		if not ptr or ffi.istype(obj_ctype, ptr) then return ptr end\n"
"		-- check if it is a compatible type.\n"
"		local ctype = tostring(ffi.typeof(ptr))\n"
"		local bcaster = _obj_subs.Epoller[ctype]\n"
"		if bcaster then\n"
"			return bcaster(ptr)\n"
"		end\n"
"		return error(\"Expected 'Epoller *'\", 2)\n"
"	end\n"
"\n"
"	function obj_type_Epoller_delete(ptr)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		local flags = nobj_obj_flags[id]\n"
"		if not flags then return nil, 0 end\n"
"		ffi.gc(ptr, nil)\n"
"		nobj_obj_flags[id] = nil\n"
"		return ptr, flags\n"
"	end\n"
"\n"
"	function obj_type_Epoller_push(ptr, flags)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		-- check weak refs\n"
"		if nobj_obj_flags[id] then return nobj_weak_objects[id] end\n"
"\n"
"		if flags ~= 0 then\n"
"			nobj_obj_flags[id] = flags\n"
"			ffi.gc(ptr, obj_mt.__gc)\n"
"		end\n"
"		nobj_weak_objects[id] = ptr\n"
"		return ptr\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"Epoller: %p, flags=%d\", self, nobj_obj_flags[obj_ptr_to_id(self)] or 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	_priv[obj_type] = obj_type_Epoller_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr, flags)\n"
"		return obj_type_Epoller_push(ffi.cast(obj_ctype,ptr), flags)\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = obj_type_Epoller_check\n"
"	obj_mt.ffi_check = obj_type_Epoller_check\n"
"end\n"
"\n"
"\n"
"local obj_type_Buffer_check =\n"
"	obj_get_interface_check(\"BufferIF\", \"Expected object with Buffer interface\")\n"
"\n"
"local obj_type_MutableBuffer_check =\n"
"	obj_get_interface_check(\"MutableBufferIF\", \"Expected object with MutableBuffer interface\")\n"
"\n"
"local obj_type_FD_check =\n"
"	obj_get_interface_check(\"FDIF\", \"Expected object with FD interface\")\n"
"\n"
"\n"
"-- Start \"Errors\" FFI interface\n"
"-- End \"Errors\" FFI interface\n"
"\n"
"-- get Errors table to map errno to error name.\n"
"local Error_names = _M.Errors\n"
"\n"
"local function error_code__errno_rc__push(err)\n"
"  local err_str\n"
"	if(-1 == err) then\n"
"		err_str = Error_names[ffi.errno()]\n"
"	end\n"
"\n"
"	return err_str\n"
"end\n"
"\n"
"\n"
"-- Start \"Epoller\" FFI interface\n"
"-- method: new\n"
"function _pub.Epoller.new(size)\n"
"  size = size or 64\n"
"  local this_flags = OBJ_UDATA_FLAG_OWN\n"
"  local self\n"
"  self = C.epoller_create(size)\n"
"  return obj_type_Epoller_push(self, this_flags)\n"
"end\n"
"register_default_constructor(_pub,\"Epoller\",_pub.Epoller.new)\n"
"\n"
"-- method: close\n"
"function _meth.Epoller.close(self)\n"
"  local self,this_flags = obj_type_Epoller_delete(self)\n"
"  if not self then return end\n"
"  C.epoller_destroy(self)\n"
"  return \n"
"end\n"
"_priv.Epoller.__gc = _meth.Epoller.close\n"
"\n"
"-- method: add\n"
"function _meth.Epoller.add(self, fd, events, id)\n"
"  \n"
"  \n"
"  \n"
"  \n"
"  local rc_epoller_add = 0\n"
"  rc_epoller_add = C.epoller_add(self, fd, events, id)\n"
"  -- check for error.\n"
"  if (-1 == rc_epoller_add) then\n"
"    return nil, error_code__errno_rc__push(rc_epoller_add)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: mod\n"
"function _meth.Epoller.mod(self, fd, events, id)\n"
"  \n"
"  \n"
"  \n"
"  \n"
"  local rc_epoller_mod = 0\n"
"  rc_epoller_mod = C.epoller_mod(self, fd, events, id)\n"
"  -- check for error.\n"
"  if (-1 == rc_epoller_mod) then\n"
"    return nil, error_code__errno_rc__push(rc_epoller_mod)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: del\n"
"function _meth.Epoller.del(self, fd)\n"
"  \n"
"  \n"
"  local rc_epoller_del = 0\n"
"  rc_epoller_del = C.epoller_del(self, fd)\n"
"  -- check for error.\n"
"  if (-1 == rc_epoller_del) then\n"
"    return nil, error_code__errno_rc__push(rc_epoller_del)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: wait\n"
"function _meth.Epoller.wait(self, events, timeout)\n"
"  \n"
"  \n"
"  local rc = 0\n"
"  rc = C.epoller_wait(self, timeout)\n"
"	if (rc > 0) then\n"
"		local idx = 1\n"
"		-- fill 'events' table with event <id, events> pairs.\n"
"		for n=0,(rc-1) do\n"
"			events[idx] = tonumber(self.events[n].data.u64)\n"
"			idx = idx + 1\n"
"			events[idx] = tonumber(self.events[n].events)\n"
"			idx = idx + 1\n"
"		end\n"
"		return rc\n"
"	elseif (rc == 0) then\n"
"		return rc\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == rc) then\n"
"    return nil, error_code__errno_rc__push(rc)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: wait_callback\n"
"function _meth.Epoller.wait_callback(self, event_cb, timeout)\n"
"  \n"
"  \n"
"  local rc = 0\n"
"  rc = C.epoller_wait(self, timeout)\n"
"	if (rc > 0) then\n"
"		-- call 'event_cb' for each <id, events> pair.\n"
"		for n=0,(rc-1) do\n"
"			event_cb(tonumber(self.events[n].data.u64), tonumber(self.events[n].events))\n"
"		end\n"
"		return rc\n"
"	elseif (rc == 0) then\n"
"		return rc\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == rc) then\n"
"    return nil, error_code__errno_rc__push(rc)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"_push.Epoller = obj_type_Epoller_push\n"
"ffi.metatype(\"Epoller\", _priv.Epoller)\n"
"-- End \"Epoller\" FFI interface\n"
"\n"
"-- method: new\n"
"function _M.new(size)\n"
"  size = size or 64\n"
"  local this_flags = OBJ_UDATA_FLAG_OWN\n"
"  local this\n"
"  this = Cmod.epoller_create(size)\n"
"  return obj_type_Epoller_push(this, this_flags)\n"
"end\n"
"\n", NULL };
static char epoll_Errors_key[] = "epoll_Errors_key";

typedef struct Epoller {
	int epfd;
	int size;
	int count;
	struct epoll_event *events;
} Epoller;

int epoller_wait(Epoller *this, int timeout);

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




/* method: description */
static int Errors__description__meth(lua_State *L) {
  size_t msg_len = 0;
  char msg_buf[1024];
  char * msg = msg_buf;
	int err_type;
	int err_num = -1;

  msg_len = 1023;
	err_type = lua_type(L, 2);
	if(err_type == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
		if(lua_isnumber(L, -1)) {
			err_num = lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	} else if(err_type == LUA_TNUMBER) {
		err_num = lua_tointeger(L, 2);
	} else {
		return luaL_argerror(L, 2, "expected string/number");
	}
	if(err_num < 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "UNKNOWN ERROR");
		return 2;
	}
	strerror_r(err_num, msg, msg_len);
	msg_len = strlen(msg);

  if(msg == NULL) lua_pushnil(L);  else lua_pushlstring(L, msg,msg_len);
  return 1;
}

static void error_code__errno_rc__push(lua_State *L, errno_rc err) {
  const char *err_str = NULL;
	if(-1 == err) {
		/* get Errors table. */
		lua_pushlightuserdata(L, epoll_Errors_key);
		lua_rawget(L, LUA_REGISTRYINDEX);
		/* convert errno to string. */
		lua_rawgeti(L, -1, errno);
		/* remove Errors table. */
		lua_remove(L, -2);
		if(!lua_isnil(L, -1)) {
			/* found error. */
			return;
		}
		/* Unknown error. */
		lua_pop(L, 1);
		err_str = "UNKNOWN ERROR";
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: new */
static int Epoller__new__meth(lua_State *L) {
  int size;
  int this_flags = OBJ_UDATA_FLAG_OWN;
  Epoller * this;
  size = luaL_optinteger(L,1,64);
  this = epoller_create(size);
  obj_type_Epoller_push(L, this, this_flags);
  return 1;
}

/* method: close */
static int Epoller__close__meth(lua_State *L) {
  int this_flags = 0;
  Epoller * this;
  this = obj_type_Epoller_delete(L,1,&(this_flags));
  if(!(this_flags & OBJ_UDATA_FLAG_OWN)) { return 0; }
  epoller_destroy(this);
  return 0;
}

/* method: add */
static int Epoller__add__meth(lua_State *L) {
  Epoller * this;
  int fd;
  uint32_t events;
  uint64_t id;
  errno_rc rc_epoller_add = 0;
  this = obj_type_Epoller_check(L,1);
  fd = luaL_checkinteger(L,2);
  events = luaL_checkinteger(L,3);
  id = luaL_checkinteger(L,4);
  rc_epoller_add = epoller_add(this, fd, events, id);
  /* check for error. */
  if((-1 == rc_epoller_add)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_epoller_add);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: mod */
static int Epoller__mod__meth(lua_State *L) {
  Epoller * this;
  int fd;
  uint32_t events;
  uint64_t id;
  errno_rc rc_epoller_mod = 0;
  this = obj_type_Epoller_check(L,1);
  fd = luaL_checkinteger(L,2);
  events = luaL_checkinteger(L,3);
  id = luaL_checkinteger(L,4);
  rc_epoller_mod = epoller_mod(this, fd, events, id);
  /* check for error. */
  if((-1 == rc_epoller_mod)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_epoller_mod);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: del */
static int Epoller__del__meth(lua_State *L) {
  Epoller * this;
  int fd;
  errno_rc rc_epoller_del = 0;
  this = obj_type_Epoller_check(L,1);
  fd = luaL_checkinteger(L,2);
  rc_epoller_del = epoller_del(this, fd);
  /* check for error. */
  if((-1 == rc_epoller_del)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc_epoller_del);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: wait */
static int Epoller__wait__meth(lua_State *L) {
  Epoller * this;
  int timeout;
  errno_rc rc = 0;
  this = obj_type_Epoller_check(L,1);
  timeout = luaL_checkinteger(L,3);
	luaL_checktype(L, 2, LUA_TTABLE);

  rc = epoller_wait(this, timeout);
	if(rc > 0) {
		int idx;
		int n;
		/* fill 'events' table with event <id, events> pairs. */
		for(n = 0, idx = 1; n < rc; n++) {
			lua_pushinteger(L, this->events[n].data.u64);
			lua_rawseti(L, 2, idx); idx++;
			lua_pushinteger(L, this->events[n].events);
			lua_rawseti(L, 2, idx); idx++;
		}
		lua_pushinteger(L, rc);
		return 1;
	} else if(rc == 0) {
		lua_pushinteger(L, rc);
		return 1;
	}

  /* check for error. */
  if((-1 == rc)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: wait_callback */
static int Epoller__wait_callback__meth(lua_State *L) {
  Epoller * this;
  int timeout;
  errno_rc rc = 0;
  this = obj_type_Epoller_check(L,1);
  timeout = luaL_checkinteger(L,3);
	luaL_checktype(L, 2, LUA_TFUNCTION);

  rc = epoller_wait(this, timeout);
	if(rc > 0) {
		int n;
		/* call 'event_cb' for each <id, events> pair. */
		for(n = 0; n < rc; n++) {
			lua_pushvalue(L,  2);
			lua_pushinteger(L, this->events[n].data.u64);
			lua_pushinteger(L, this->events[n].events);
			lua_call(L, 2, 0);
		}
		lua_pushinteger(L, rc);
		return 1;
	} else if(rc == 0) {
		lua_pushinteger(L, rc);
		return 1;
	}

  /* check for error. */
  if((-1 == rc)) {
    lua_pushnil(L);
      error_code__errno_rc__push(L, rc);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: new */
static int epoll__new__func(lua_State *L) {
  int size;
  int this_flags = OBJ_UDATA_FLAG_OWN;
  Epoller * this;
  size = luaL_optinteger(L,1,64);
  this = epoller_create(size);
  obj_type_Epoller_push(L, this, this_flags);
  return 1;
}



static const luaL_Reg obj_Errors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_methods[] = {
  {"description", Errors__description__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_metas[] = {
  {NULL, NULL}
};

static const obj_const obj_Errors_constants[] = {
#ifdef ELNRNG
  {"ELNRNG", NULL, ELNRNG, CONST_NUMBER},
#endif
#ifdef EISDIR
  {"EISDIR", NULL, EISDIR, CONST_NUMBER},
#endif
#ifdef EFBIG
  {"EFBIG", NULL, EFBIG, CONST_NUMBER},
#endif
#ifdef EDQUOT
  {"EDQUOT", NULL, EDQUOT, CONST_NUMBER},
#endif
#ifdef EIO
  {"EIO", NULL, EIO, CONST_NUMBER},
#endif
#ifdef EBUSY
  {"EBUSY", NULL, EBUSY, CONST_NUMBER},
#endif
#ifdef ENOEXEC
  {"ENOEXEC", NULL, ENOEXEC, CONST_NUMBER},
#endif
#ifdef ENAVAIL
  {"ENAVAIL", NULL, ENAVAIL, CONST_NUMBER},
#endif
#ifdef EEXIST
  {"EEXIST", NULL, EEXIST, CONST_NUMBER},
#endif
#ifdef EPIPE
  {"EPIPE", NULL, EPIPE, CONST_NUMBER},
#endif
#ifdef ELIBEXEC
  {"ELIBEXEC", NULL, ELIBEXEC, CONST_NUMBER},
#endif
#ifdef ENFILE
  {"ENFILE", NULL, ENFILE, CONST_NUMBER},
#endif
#ifdef ENOMEDIUM
  {"ENOMEDIUM", NULL, ENOMEDIUM, CONST_NUMBER},
#endif
#ifdef EREMOTE
  {"EREMOTE", NULL, EREMOTE, CONST_NUMBER},
#endif
#ifdef ESPIPE
  {"ESPIPE", NULL, ESPIPE, CONST_NUMBER},
#endif
#ifdef EBADFD
  {"EBADFD", NULL, EBADFD, CONST_NUMBER},
#endif
#ifdef ENOENT
  {"ENOENT", NULL, ENOENT, CONST_NUMBER},
#endif
#ifdef EBADMSG
  {"EBADMSG", NULL, EBADMSG, CONST_NUMBER},
#endif
#ifdef ENOTCONN
  {"ENOTCONN", NULL, ENOTCONN, CONST_NUMBER},
#endif
#ifdef ENOPKG
  {"ENOPKG", NULL, ENOPKG, CONST_NUMBER},
#endif
#ifdef EACCES
  {"EACCES", NULL, EACCES, CONST_NUMBER},
#endif
#ifdef EL3HLT
  {"EL3HLT", NULL, EL3HLT, CONST_NUMBER},
#endif
#ifdef EL2NSYNC
  {"EL2NSYNC", NULL, EL2NSYNC, CONST_NUMBER},
#endif
#ifdef EDEADLOCK
  {"EDEADLOCK", NULL, EDEADLOCK, CONST_NUMBER},
#endif
#ifdef ENOTEMPTY
  {"ENOTEMPTY", NULL, ENOTEMPTY, CONST_NUMBER},
#endif
#ifdef ERESTART
  {"ERESTART", NULL, ERESTART, CONST_NUMBER},
#endif
#ifdef EILSEQ
  {"EILSEQ", NULL, EILSEQ, CONST_NUMBER},
#endif
#ifdef EKEYREJECTED
  {"EKEYREJECTED", NULL, EKEYREJECTED, CONST_NUMBER},
#endif
#ifdef EDEADLK
  {"EDEADLK", NULL, EDEADLK, CONST_NUMBER},
#endif
#ifdef EBADF
  {"EBADF", NULL, EBADF, CONST_NUMBER},
#endif
#ifdef ECHILD
  {"ECHILD", NULL, ECHILD, CONST_NUMBER},
#endif
#ifdef EALREADY
  {"EALREADY", NULL, EALREADY, CONST_NUMBER},
#endif
#ifdef ENETDOWN
  {"ENETDOWN", NULL, ENETDOWN, CONST_NUMBER},
#endif
#ifdef ENOLCK
  {"ENOLCK", NULL, ENOLCK, CONST_NUMBER},
#endif
#ifdef ENOTSOCK
  {"ENOTSOCK", NULL, ENOTSOCK, CONST_NUMBER},
#endif
#ifdef ELIBSCN
  {"ELIBSCN", NULL, ELIBSCN, CONST_NUMBER},
#endif
#ifdef ENAMETOOLONG
  {"ENAMETOOLONG", NULL, ENAMETOOLONG, CONST_NUMBER},
#endif
#ifdef EBFONT
  {"EBFONT", NULL, EBFONT, CONST_NUMBER},
#endif
#ifdef ETXTBSY
  {"ETXTBSY", NULL, ETXTBSY, CONST_NUMBER},
#endif
#ifdef EAFNOSUPPORT
  {"EAFNOSUPPORT", NULL, EAFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef EADV
  {"EADV", NULL, EADV, CONST_NUMBER},
#endif
#ifdef EUSERS
  {"EUSERS", NULL, EUSERS, CONST_NUMBER},
#endif
#ifdef ENOKEY
  {"ENOKEY", NULL, ENOKEY, CONST_NUMBER},
#endif
#ifdef ETIME
  {"ETIME", NULL, ETIME, CONST_NUMBER},
#endif
#ifdef EBADR
  {"EBADR", NULL, EBADR, CONST_NUMBER},
#endif
#ifdef ERFKILL
  {"ERFKILL", NULL, ERFKILL, CONST_NUMBER},
#endif
#ifdef ECONNABORTED
  {"ECONNABORTED", NULL, ECONNABORTED, CONST_NUMBER},
#endif
#ifdef EREMOTEIO
  {"EREMOTEIO", NULL, EREMOTEIO, CONST_NUMBER},
#endif
#ifdef EIDRM
  {"EIDRM", NULL, EIDRM, CONST_NUMBER},
#endif
#ifdef ENOTTY
  {"ENOTTY", NULL, ENOTTY, CONST_NUMBER},
#endif
#ifdef ECONNRESET
  {"ECONNRESET", NULL, ECONNRESET, CONST_NUMBER},
#endif
#ifdef EADDRINUSE
  {"EADDRINUSE", NULL, EADDRINUSE, CONST_NUMBER},
#endif
#ifdef EKEYREVOKED
  {"EKEYREVOKED", NULL, EKEYREVOKED, CONST_NUMBER},
#endif
#ifdef ENOMEM
  {"ENOMEM", NULL, ENOMEM, CONST_NUMBER},
#endif
#ifdef EOWNERDEAD
  {"EOWNERDEAD", NULL, EOWNERDEAD, CONST_NUMBER},
#endif
#ifdef EROFS
  {"EROFS", NULL, EROFS, CONST_NUMBER},
#endif
#ifdef EMEDIUMTYPE
  {"EMEDIUMTYPE", NULL, EMEDIUMTYPE, CONST_NUMBER},
#endif
#ifdef ENOSPC
  {"ENOSPC", NULL, ENOSPC, CONST_NUMBER},
#endif
#ifdef ESRMNT
  {"ESRMNT", NULL, ESRMNT, CONST_NUMBER},
#endif
#ifdef ENOBUFS
  {"ENOBUFS", NULL, ENOBUFS, CONST_NUMBER},
#endif
#ifdef EINTR
  {"EINTR", NULL, EINTR, CONST_NUMBER},
#endif
#ifdef ENOMSG
  {"ENOMSG", NULL, ENOMSG, CONST_NUMBER},
#endif
#ifdef ENOCSI
  {"ENOCSI", NULL, ENOCSI, CONST_NUMBER},
#endif
#ifdef EPERM
  {"EPERM", NULL, EPERM, CONST_NUMBER},
#endif
#ifdef ENOSR
  {"ENOSR", NULL, ENOSR, CONST_NUMBER},
#endif
#ifdef EREMCHG
  {"EREMCHG", NULL, EREMCHG, CONST_NUMBER},
#endif
#ifdef EPFNOSUPPORT
  {"EPFNOSUPPORT", NULL, EPFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef EL3RST
  {"EL3RST", NULL, EL3RST, CONST_NUMBER},
#endif
#ifdef EMULTIHOP
  {"EMULTIHOP", NULL, EMULTIHOP, CONST_NUMBER},
#endif
#ifdef EAGAIN
  {"EAGAIN", NULL, EAGAIN, CONST_NUMBER},
#endif
#ifdef ENODEV
  {"ENODEV", NULL, ENODEV, CONST_NUMBER},
#endif
#ifdef ELIBBAD
  {"ELIBBAD", NULL, ELIBBAD, CONST_NUMBER},
#endif
#ifdef ENOSTR
  {"ENOSTR", NULL, ENOSTR, CONST_NUMBER},
#endif
#ifdef ELOOP
  {"ELOOP", NULL, ELOOP, CONST_NUMBER},
#endif
#ifdef ECONNREFUSED
  {"ECONNREFUSED", NULL, ECONNREFUSED, CONST_NUMBER},
#endif
#ifdef ENOPROTOOPT
  {"ENOPROTOOPT", NULL, ENOPROTOOPT, CONST_NUMBER},
#endif
#ifdef ETIMEDOUT
  {"ETIMEDOUT", NULL, ETIMEDOUT, CONST_NUMBER},
#endif
#ifdef EUCLEAN
  {"EUCLEAN", NULL, EUCLEAN, CONST_NUMBER},
#endif
#ifdef ESOCKTNOSUPPORT
  {"ESOCKTNOSUPPORT", NULL, ESOCKTNOSUPPORT, CONST_NUMBER},
#endif
#ifdef ETOOMANYREFS
  {"ETOOMANYREFS", NULL, ETOOMANYREFS, CONST_NUMBER},
#endif
#ifdef EOVERFLOW
  {"EOVERFLOW", NULL, EOVERFLOW, CONST_NUMBER},
#endif
#ifdef EINPROGRESS
  {"EINPROGRESS", NULL, EINPROGRESS, CONST_NUMBER},
#endif
#ifdef ENOTDIR
  {"ENOTDIR", NULL, ENOTDIR, CONST_NUMBER},
#endif
#ifdef EKEYEXPIRED
  {"EKEYEXPIRED", NULL, EKEYEXPIRED, CONST_NUMBER},
#endif
#ifdef E2BIG
  {"E2BIG", NULL, E2BIG, CONST_NUMBER},
#endif
#ifdef ECHRNG
  {"ECHRNG", NULL, ECHRNG, CONST_NUMBER},
#endif
#ifdef EBADSLT
  {"EBADSLT", NULL, EBADSLT, CONST_NUMBER},
#endif
#ifdef ECANCELED
  {"ECANCELED", NULL, ECANCELED, CONST_NUMBER},
#endif
#ifdef ENOTRECOVERABLE
  {"ENOTRECOVERABLE", NULL, ENOTRECOVERABLE, CONST_NUMBER},
#endif
#ifdef ERANGE
  {"ERANGE", NULL, ERANGE, CONST_NUMBER},
#endif
#ifdef ENOSYS
  {"ENOSYS", NULL, ENOSYS, CONST_NUMBER},
#endif
#ifdef EXFULL
  {"EXFULL", NULL, EXFULL, CONST_NUMBER},
#endif
#ifdef EISNAM
  {"EISNAM", NULL, EISNAM, CONST_NUMBER},
#endif
#ifdef ENOTUNIQ
  {"ENOTUNIQ", NULL, ENOTUNIQ, CONST_NUMBER},
#endif
#ifdef ENXIO
  {"ENXIO", NULL, ENXIO, CONST_NUMBER},
#endif
#ifdef ESTALE
  {"ESTALE", NULL, ESTALE, CONST_NUMBER},
#endif
#ifdef EPROTOTYPE
  {"EPROTOTYPE", NULL, EPROTOTYPE, CONST_NUMBER},
#endif
#ifdef ENOTNAM
  {"ENOTNAM", NULL, ENOTNAM, CONST_NUMBER},
#endif
#ifdef ESTRPIPE
  {"ESTRPIPE", NULL, ESTRPIPE, CONST_NUMBER},
#endif
#ifdef EMSGSIZE
  {"EMSGSIZE", NULL, EMSGSIZE, CONST_NUMBER},
#endif
#ifdef ELIBACC
  {"ELIBACC", NULL, ELIBACC, CONST_NUMBER},
#endif
#ifdef EHOSTUNREACH
  {"EHOSTUNREACH", NULL, EHOSTUNREACH, CONST_NUMBER},
#endif
#ifdef EDOM
  {"EDOM", NULL, EDOM, CONST_NUMBER},
#endif
#ifdef EMLINK
  {"EMLINK", NULL, EMLINK, CONST_NUMBER},
#endif
#ifdef ESHUTDOWN
  {"ESHUTDOWN", NULL, ESHUTDOWN, CONST_NUMBER},
#endif
#ifdef EHOSTDOWN
  {"EHOSTDOWN", NULL, EHOSTDOWN, CONST_NUMBER},
#endif
#ifdef EPROTO
  {"EPROTO", NULL, EPROTO, CONST_NUMBER},
#endif
#ifdef ENONET
  {"ENONET", NULL, ENONET, CONST_NUMBER},
#endif
#ifdef EADDRNOTAVAIL
  {"EADDRNOTAVAIL", NULL, EADDRNOTAVAIL, CONST_NUMBER},
#endif
#ifdef ENODATA
  {"ENODATA", NULL, ENODATA, CONST_NUMBER},
#endif
#ifdef ECOMM
  {"ECOMM", NULL, ECOMM, CONST_NUMBER},
#endif
#ifdef EPROTONOSUPPORT
  {"EPROTONOSUPPORT", NULL, EPROTONOSUPPORT, CONST_NUMBER},
#endif
#ifdef EISCONN
  {"EISCONN", NULL, EISCONN, CONST_NUMBER},
#endif
#ifdef EBADE
  {"EBADE", NULL, EBADE, CONST_NUMBER},
#endif
#ifdef EINVAL
  {"EINVAL", NULL, EINVAL, CONST_NUMBER},
#endif
#ifdef ENETRESET
  {"ENETRESET", NULL, ENETRESET, CONST_NUMBER},
#endif
#ifdef ENETUNREACH
  {"ENETUNREACH", NULL, ENETUNREACH, CONST_NUMBER},
#endif
#ifdef ENOLINK
  {"ENOLINK", NULL, ENOLINK, CONST_NUMBER},
#endif
#ifdef EWOULDBLOCK
  {"EWOULDBLOCK", NULL, EWOULDBLOCK, CONST_NUMBER},
#endif
#ifdef ENOANO
  {"ENOANO", NULL, ENOANO, CONST_NUMBER},
#endif
#ifdef EFAULT
  {"EFAULT", NULL, EFAULT, CONST_NUMBER},
#endif
#ifdef EDESTADDRREQ
  {"EDESTADDRREQ", NULL, EDESTADDRREQ, CONST_NUMBER},
#endif
#ifdef ELIBMAX
  {"ELIBMAX", NULL, ELIBMAX, CONST_NUMBER},
#endif
#ifdef EDOTDOT
  {"EDOTDOT", NULL, EDOTDOT, CONST_NUMBER},
#endif
#ifdef EBADRQC
  {"EBADRQC", NULL, EBADRQC, CONST_NUMBER},
#endif
#ifdef ESRCH
  {"ESRCH", NULL, ESRCH, CONST_NUMBER},
#endif
#ifdef EOPNOTSUPP
  {"EOPNOTSUPP", NULL, EOPNOTSUPP, CONST_NUMBER},
#endif
#ifdef EMFILE
  {"EMFILE", NULL, EMFILE, CONST_NUMBER},
#endif
#ifdef EL2HLT
  {"EL2HLT", NULL, EL2HLT, CONST_NUMBER},
#endif
#ifdef ENOTBLK
  {"ENOTBLK", NULL, ENOTBLK, CONST_NUMBER},
#endif
#ifdef EUNATCH
  {"EUNATCH", NULL, EUNATCH, CONST_NUMBER},
#endif
#ifdef EXDEV
  {"EXDEV", NULL, EXDEV, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Errors_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Epoller_pub_funcs[] = {
  {"new", Epoller__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Epoller_methods[] = {
  {"close", Epoller__close__meth},
  {"add", Epoller__add__meth},
  {"mod", Epoller__mod__meth},
  {"del", Epoller__del__meth},
  {"wait", Epoller__wait__meth},
  {"wait_callback", Epoller__wait_callback__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Epoller_metas[] = {
  {"__gc", Epoller__close__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Epoller_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Epoller_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Epoller_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Epoller_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg epoll_function[] = {
  {"new", epoll__new__func},
  {NULL, NULL}
};

static const obj_const epoll_constants[] = {
#ifdef EPOLLERR
  {"EPOLLERR", NULL, EPOLLERR, CONST_NUMBER},
#endif
#ifdef EPOLLIN
  {"EPOLLIN", NULL, EPOLLIN, CONST_NUMBER},
#endif
#ifdef EPOLLONESHOT
  {"EPOLLONESHOT", NULL, EPOLLONESHOT, CONST_NUMBER},
#endif
#ifdef EPOLLOUT
  {"EPOLLOUT", NULL, EPOLLOUT, CONST_NUMBER},
#endif
#ifdef EPOLLET
  {"EPOLLET", NULL, EPOLLET, CONST_NUMBER},
#endif
#ifdef EPOLLPRI
  {"EPOLLPRI", NULL, EPOLLPRI, CONST_NUMBER},
#endif
#ifdef EPOLLRDHUP
  {"EPOLLRDHUP", NULL, EPOLLRDHUP, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Errors), REG_META, obj_Errors_pub_funcs, obj_Errors_methods, obj_Errors_metas, NULL, NULL, obj_Errors_constants, NULL, 1},
  { &(obj_type_Epoller), REG_OBJECT, obj_Epoller_pub_funcs, obj_Epoller_methods, obj_Epoller_metas, obj_Epoller_bases, obj_Epoller_fields, obj_Epoller_constants, obj_Epoller_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol epoll_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_epoll(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "epoll", epoll_function);
#else
	lua_newtable(L);
	nobj_setfuncs(L, epoll_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, epoll_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "epoll.nobj.ffi.lua", epoll_ffi_lua_code,
			epoll_ffi_export, priv_table);
	}
#endif

	/* Cache reference to epoll.Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, epoll_Errors_key);
	lua_getfield(L, -2, "Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);



	return 1;
}


