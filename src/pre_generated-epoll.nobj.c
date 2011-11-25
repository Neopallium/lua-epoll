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

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define LUAJIT_FFI 1
#define USE_FIELD_GET_SET_METHODS 0


#include <sys/epoll.h>
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
#define false 1
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

#ifndef obj_type_free
#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	free((obj)); \
} while(0)
#endif

#ifndef obj_type_new
#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = malloc(sizeof(type)); \
} while(0)
#endif

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
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

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_reg  *pub_funcs;
	const luaL_reg  *methods;
	const luaL_reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	bool            bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char *obj_udata_weak_ref_key = "obj_udata_weak_ref_key";

#if LUAJIT_FFI
typedef struct ffi_export_symbol {
	const char *name;
	void       *sym;
} ffi_export_symbol;
#endif




static obj_type obj_types[] = {
#define obj_type_id_Epoller 0
#define obj_type_Epoller (obj_types[obj_type_id_Epoller])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Epoller" },
  {NULL, -1, 0, NULL},
};


#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_objlen(L, _index);
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
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
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
	if(lua_isnil(L, _index)) {
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
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
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
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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

	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	*flags = OBJ_UDATA_FLAG_OWN;
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	/* create new userdata. */
	void *ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_objlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_objlen(L, 2);

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
		bool bidirectional) {
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
	const luaL_reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	luaL_register(L, NULL, type_reg->methods);

	/* create metatable table. */
	lua_newtable(L);
	luaL_register(L, NULL, type_reg->metas); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_reg *reg_list;
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
		luaL_register(L, NULL, reg_list); /* fill public API table. */

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

	luaL_register(L, NULL, type_reg->methods); /* fill methods table. */

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

#if LUAJIT_FFI
	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */
#else
	(void)priv_table;
#endif

	luaL_register(L, NULL, type_reg->metas); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

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

#if LUAJIT_FFI
static int nobj_udata_new_ffi(lua_State *L) {
	size_t size = luaL_checkinteger(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_settop(L, 2);
	/* create userdata. */
	lua_newuserdata(L, size);
	lua_replace(L, 1);
	/* set userdata's metatable. */
	lua_setmetatable(L, 1);
	return 1;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code, const ffi_export_symbol *ffi_exports, int priv_table)
{
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = luaL_loadbuffer(L, ffi_init_code, strlen(ffi_init_code), ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushcfunction(L, nobj_udata_new_ffi);
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


#define obj_type_Epoller_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Epoller))
#define obj_type_Epoller_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Epoller))
#define obj_type_Epoller_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Epoller), flags)
#define obj_type_Epoller_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Epoller), flags)




static const char epoll_ffi_lua_code[] = "local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local rawset = rawset\n"
"\n"
"-- try loading luajit's ffi\n"
"local stat, ffi=pcall(require,\"ffi\")\n"
"if not stat then\n"
"	return\n"
"end\n"
"\n"
"local _M, _priv, udata_new = ...\n"
"\n"
"local band = bit.band\n"
"local d_getmetatable = debug.getmetatable\n"
"local d_setmetatable = debug.setmetatable\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"local OBJ_UDATA_FLAG_LOOKUP	= 2\n"
"local OBJ_UDATA_LAST_FLAG		= OBJ_UDATA_FLAG_LOOKUP\n"
"\n"
"local OBJ_TYPE_FLAG_WEAK_REF	= 1\n"
"local OBJ_TYPE_SIMPLE					= 2\n"
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
"]])\n"
"\n"
"-- cache mapping of cdata to userdata\n"
"local weak_objects = setmetatable({}, { __mode = \"v\" })\n"
"\n"
"local function obj_udata_luacheck_internal(obj, type_mt, not_delete)\n"
"	local obj_mt = d_getmetatable(obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"obj_udata *\", obj)\n"
"	end\n"
"	if not_delete then\n"
"		error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(obj) .. \")\", 3)\n"
"	end\n"
"end\n"
"\n"
"local function obj_udata_luacheck(obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(obj, type_mt, true)\n"
"	return ud.obj\n"
"end\n"
"\n"
"local function obj_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete_weak(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	-- remove object from weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	weak_objects[obj_key] = nil\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush_weak(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- lookup object in weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	local ud_obj = weak_objects[obj_key]\n"
"	if ud_obj ~= nil then return ud_obj end\n"
"\n"
"	-- create new userdata\n"
"	ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	-- cache weak reference to object.\n"
"	weak_objects[obj_key] = ud_obj\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luacheck(ud_obj, type_mt)\n"
"	local obj_mt = d_getmetatable(ud_obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"void *\", ud_obj)\n"
"	end\n"
"	error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(ud_obj) .. \")\", 3)\n"
"end\n"
"\n"
"local function obj_simple_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_simple_udata_luacheck(ud_obj, ud_mt))[0]\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_embed_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_simple_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luadelete(ud_obj, type_mt)\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	return OBJ_UDATA_FLAG_OWN\n"
"end\n"
"\n"
"local function obj_simple_udata_luapush(c_obj, size, type_mt)\n"
"	if c_obj == nil then return end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(size, type_mt)\n"
"	local cdata = ffi.cast(\"void *\", ud_obj)\n"
"	-- init. object\n"
"	ffi.copy(cdata, c_obj, size)\n"
"\n"
"	return ud_obj, cdata\n"
"end\n"
"\n"
"ffi.cdef[[\n"
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
"typedef int (*epoller_add_func)(Epoller *this, int fd, uint32_t events, uint64_t id);\n"
"\n"
"typedef int (*epoller_mod_func)(Epoller *this, int fd, uint32_t events, uint64_t id);\n"
"\n"
"typedef int (*epoller_del_func)(Epoller *this, int fd);\n"
"\n"
"typedef int (*epoller_wait_func)(Epoller *this, int timeout);\n"
"\n"
"\n"
"]]\n"
"\n"
"local _pub = {}\n"
"local _meth = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' and mt.__index then\n"
"		_meth[obj_name] = mt.__index\n"
"	end\n"
"end\n"
"_pub.epoll = _M\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"\n"
"local obj_type_Epoller_check\n"
"local obj_type_Epoller_delete\n"
"local obj_type_Epoller_push\n"
"\n"
"do\n"
"local Epoller_mt = _priv.Epoller\n"
"local Epoller_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_udata_to_cdata(objects, ud_obj, \"Epoller *\", Epoller_mt)\n"
"end,\n"
"})\n"
"function obj_type_Epoller_check(ud_obj)\n"
"	return Epoller_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_Epoller_delete(ud_obj)\n"
"	Epoller_objects[ud_obj] = nil\n"
"	return obj_udata_luadelete_weak(ud_obj, Epoller_mt)\n"
"end\n"
"\n"
"local Epoller_type = ffi.cast(\"obj_type *\", Epoller_mt[\".type\"])\n"
"function obj_type_Epoller_push(c_obj, flags)\n"
"	local ud_obj = obj_udata_luapush_weak(c_obj, Epoller_mt, Epoller_type, flags)\n"
"	Epoller_objects[ud_obj] = c_obj\n"
"	return ud_obj\n"
"end\n"
"end\n"
"\n"
"\n"
"local epoller_add = ffi.new(\"epoller_add_func\", _priv[\"epoller_add\"])\n"
"\n"
"local epoller_mod = ffi.new(\"epoller_mod_func\", _priv[\"epoller_mod\"])\n"
"\n"
"local epoller_del = ffi.new(\"epoller_del_func\", _priv[\"epoller_del\"])\n"
"\n"
"local epoller_wait = ffi.new(\"epoller_wait_func\", _priv[\"epoller_wait\"])\n"
"\n"
"\n"
"-- Start \"Epoller\" FFI interface\n"
"-- method: add\n"
"function _meth.Epoller.add(self, fd, events, id)\n"
"  local this = obj_type_Epoller_check(self)\n"
"  \n"
"  \n"
"  \n"
"  local rc\n"
"	rc = epoller_add(this, fd, events, id);\n"
"\n"
"  rc = rc\n"
"  return rc\n"
"end\n"
"\n"
"-- method: mod\n"
"function _meth.Epoller.mod(self, fd, events, id)\n"
"  local this = obj_type_Epoller_check(self)\n"
"  \n"
"  \n"
"  \n"
"  local rc\n"
"	rc = epoller_mod(this, fd, events, id);\n"
"\n"
"  rc = rc\n"
"  return rc\n"
"end\n"
"\n"
"-- method: del\n"
"function _meth.Epoller.del(self, fd)\n"
"  local this = obj_type_Epoller_check(self)\n"
"  \n"
"  local rc\n"
"	rc = epoller_del(this, fd);\n"
"\n"
"  rc = rc\n"
"  return rc\n"
"end\n"
"\n"
"-- method: wait\n"
"function _meth.Epoller.wait(self, events, timeout)\n"
"  local this = obj_type_Epoller_check(self)\n"
"  \n"
"  local rc\n"
"	rc = epoller_wait(this, timeout);\n"
"	if (rc > 0) then\n"
"		local idx = 1\n"
"		-- fill 'events' table with event <id, events> pairs.\n"
"		for n=0,(rc-1) do\n"
"			events[idx] = tonumber(this.events[n].data.u64)\n"
"			idx = idx + 1\n"
"			events[idx] = tonumber(this.events[n].events)\n"
"			idx = idx + 1\n"
"		end\n"
"	end\n"
"\n"
"  rc = rc\n"
"  return rc\n"
"end\n"
"\n"
"-- method: wait_callback\n"
"function _meth.Epoller.wait_callback(self, event_cb, timeout)\n"
"  local this = obj_type_Epoller_check(self)\n"
"  \n"
"  local rc\n"
"	rc = epoller_wait(this, timeout);\n"
"	if (rc > 0) then\n"
"		-- call 'event_cb' for each <id, events> pair.\n"
"		for n=0,(rc-1) do\n"
"			event_cb(tonumber(this.events[n].data.u64), tonumber(this.events[n].events))\n"
"		end\n"
"	end\n"
"\n"
"  rc = rc\n"
"  return rc\n"
"end\n"
"\n"
"-- End \"Epoller\" FFI interface\n"
"\n"
"";

typedef struct Epoller {
	int epfd;
	int size;
	int count;
	struct epoll_event *events;
} Epoller;

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




/* method: new */
static int Epoller__new__meth(lua_State *L) {
  int size = luaL_optinteger(L,1,64);
  int this_flags = OBJ_UDATA_FLAG_OWN;
  Epoller * this;
	this = epoller_create(size);

  obj_type_Epoller_push(L, this, this_flags);
  return 1;
}

/* method: delete */
static int Epoller__delete__meth(lua_State *L) {
  int this_flags = 0;
  Epoller * this = obj_type_Epoller_delete(L,1,&(this_flags));
  if(!(this_flags & OBJ_UDATA_FLAG_OWN)) { return 0; }
	epoller_destroy(this);

  return 0;
}

/* method: add */
static int Epoller__add__meth(lua_State *L) {
  Epoller * this = obj_type_Epoller_check(L,1);
  int fd = luaL_checkinteger(L,2);
  uint32_t events = luaL_checkinteger(L,3);
  uint64_t id = luaL_checkinteger(L,4);
  int rc = 0;
	rc = epoller_add(this, fd, events, id);

  lua_pushinteger(L, rc);
  return 1;
}

/* method: mod */
static int Epoller__mod__meth(lua_State *L) {
  Epoller * this = obj_type_Epoller_check(L,1);
  int fd = luaL_checkinteger(L,2);
  uint32_t events = luaL_checkinteger(L,3);
  uint64_t id = luaL_checkinteger(L,4);
  int rc = 0;
	rc = epoller_mod(this, fd, events, id);

  lua_pushinteger(L, rc);
  return 1;
}

/* method: del */
static int Epoller__del__meth(lua_State *L) {
  Epoller * this = obj_type_Epoller_check(L,1);
  int fd = luaL_checkinteger(L,2);
  int rc = 0;
	rc = epoller_del(this, fd);

  lua_pushinteger(L, rc);
  return 1;
}

/* method: wait */
static int Epoller__wait__meth(lua_State *L) {
  Epoller * this = obj_type_Epoller_check(L,1);
  int timeout = luaL_checkinteger(L,3);
  int rc = 0;
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
	}

  lua_pushinteger(L, rc);
  return 1;
}

/* method: wait_callback */
static int Epoller__wait_callback__meth(lua_State *L) {
  Epoller * this = obj_type_Epoller_check(L,1);
  int timeout = luaL_checkinteger(L,3);
  int rc = 0;
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
	}

  lua_pushinteger(L, rc);
  return 1;
}

/* method: new */
static int epoll__new__func(lua_State *L) {
  int size = luaL_optinteger(L,1,64);
  int this_flags = OBJ_UDATA_FLAG_OWN;
  Epoller * this;
	this = epoller_create(size);

  obj_type_Epoller_push(L, this, this_flags);
  return 1;
}



static const luaL_reg obj_Epoller_pub_funcs[] = {
  {"new", Epoller__new__meth},
  {NULL, NULL}
};

static const luaL_reg obj_Epoller_methods[] = {
  {"add", Epoller__add__meth},
  {"mod", Epoller__mod__meth},
  {"del", Epoller__del__meth},
  {"wait", Epoller__wait__meth},
  {"wait_callback", Epoller__wait_callback__meth},
  {NULL, NULL}
};

static const luaL_reg obj_Epoller_metas[] = {
  {"__gc", Epoller__delete__meth},
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

static const luaL_reg epoll_function[] = {
  {"new", epoll__new__func},
  {NULL, NULL}
};

static const obj_const epoll_constants[] = {
#ifdef EPOLLPRI
  {"EPOLLPRI", NULL, EPOLLPRI, CONST_NUMBER},
#endif
#ifdef EPOLLRDHUP
  {"EPOLLRDHUP", NULL, EPOLLRDHUP, CONST_NUMBER},
#endif
#ifdef EPOLLERR
  {"EPOLLERR", NULL, EPOLLERR, CONST_NUMBER},
#endif
#ifdef EPOLLIN
  {"EPOLLIN", NULL, EPOLLIN, CONST_NUMBER},
#endif
#ifdef EPOLLOUT
  {"EPOLLOUT", NULL, EPOLLOUT, CONST_NUMBER},
#endif
#ifdef EPOLLET
  {"EPOLLET", NULL, EPOLLET, CONST_NUMBER},
#endif
#ifdef EPOLLONESHOT
  {"EPOLLONESHOT", NULL, EPOLLONESHOT, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const ffi_export_symbol epoll_ffi_export[] = {
{ "epoller_add", epoller_add },
{ "epoller_mod", epoller_mod },
{ "epoller_del", epoller_del },
{ "epoller_wait", epoller_wait },
  {NULL, NULL}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Epoller), REG_OBJECT, obj_Epoller_pub_funcs, obj_Epoller_methods, obj_Epoller_metas, obj_Epoller_bases, obj_Epoller_fields, obj_Epoller_constants, false},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, false}
};







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

#if LUAJIT_FFI
	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
#endif

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
	luaL_register(L, "epoll", epoll_function);

	/* register module constants. */
	obj_type_register_constants(L, epoll_constants, -1, false);

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
	nobj_try_loading_ffi(L, "epoll", epoll_ffi_lua_code,
		epoll_ffi_export, priv_table);
#endif
	return 1;
}


