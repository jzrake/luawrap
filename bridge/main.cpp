
#include <iostream>
#include "lua.hpp"

#ifdef __GNUC__
#include <cstdlib>
#include <cxxabi.h>
std::string demangle(const char *mname)
{
  static int status;
  char *realname = abi::__cxa_demangle(mname, 0, 0, &status);
  std::string ret = realname;
  free(realname);
  return ret;
}
#else
std::string demangle(const char *mname)
{
  return std::string(mname);
}
#endif // __GNUC__


#define LuaObject_REGISTRY   "__LuaObject_REGISTRY"
#define LuaObject_METATABLE  "__LuaObject_METATABLE"


/*
 *
 *   LuaObject_REGISTRY = { [pure Lua object]: C++ LuaObject[fulluserdata] }
 *
 * + has weak values
 * + used to check for an existing LuaObject associated to the pure Lua object
 *
 *
 *   LUA_REGISTRYINDEX = { C++ LuaObject[lightuserdata]: [pure Lua object] }
 *
 * + used to push the pure Lua object onto the stack, given its C++ wrapper
 * + only one C++ LuaObject per pure Lua object, ensured by LuaObject::New
 *
 */

class LuaObject
{
protected:
  lua_State *L;
public:
  static void Init(lua_State *L)
  {
    lua_newtable(L); // create the LuaObject_REGISTRY
    lua_newtable(L); // create its metatable
    lua_pushstring(L, "v"); // LuaObject_REGISTRY needs weak values:
    lua_setfield(L, -2, "__mode"); // meta(LuaObject_REGISTRY)[__mode] = "v"
    lua_setmetatable(L, -2); // assign to the LuaObject_REGISTRY's metatable
    lua_setfield(L, LUA_REGISTRYINDEX, LuaObject_REGISTRY); // stack now empty

    luaL_newmetatable(L, LuaObject_METATABLE);
    lua_pushcfunction(L, __gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1); // stack now empty
  }
  LuaObject(lua_State *L, int pos) : L(L)
  {
    lua_pushvalue(L, pos);
    lua_rawsetp(L, LUA_REGISTRYINDEX, this);
    lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_REGISTRY);
    lua_rawgetp(L, LUA_REGISTRYINDEX, this); // Lua object is the key
    *((LuaObject**) lua_newuserdata(L, sizeof(LuaObject*))) = this;
    luaL_setmetatable(L, LuaObject_METATABLE); // does not change stack

    /*
     * stack order is:
     *
     * LuaObject_REGISTRY
     * Lua object wrapped by this object
     * new userdata
     */

    lua_settable(L, -3);
    lua_pop(L, 1);
  }
  virtual ~LuaObject()
  {
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, this);
  }
  void push()
  {
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
  }
  int type()
  {
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    int t = lua_type(L, -1);
    lua_pop(L, 1);
    return t;
  }
  static LuaObject *New(lua_State *L, int pos);
  static int __gc(lua_State *L)
  {
    LuaObject *self = *static_cast<LuaObject**>(lua_touserdata(L, -1));
    delete self;
    return 0;
  }
} ;

class LuaFunction : public LuaObject
{
public:
  LuaFunction(lua_State *L, int pos) : LuaObject(L, pos) { }
  int call()
  {
    int start = lua_gettop(L);
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L) - start;
  }
} ;

class LuaNumber : public LuaObject
{
public:
  LuaNumber(lua_State *L, int pos) : LuaObject(L, pos) { }
  double get_value()
  {
    double ret;
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    ret = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ret;
  }
} ;

class LuaTable : public LuaObject
{
public:
  LuaTable(lua_State *L, int pos) : LuaObject(L, pos) { }
  void set(LuaObject *key, LuaObject *val)
  {
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    lua_rawgetp(L, LUA_REGISTRYINDEX, key);
    lua_rawgetp(L, LUA_REGISTRYINDEX, val);
    lua_settable(L, -3);
    lua_pop(L, 1);
  }
} ;


LuaObject *LuaObject::New(lua_State *L, int pos)
{
  pos = lua_absindex(L, pos);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_REGISTRY);
  lua_pushvalue(L, pos);
  lua_gettable(L, -2);

  void *udata = lua_touserdata(L, -1);
  int type = lua_type(L, pos);

  lua_pop(L, 2); // remove LuaObject_REGISTRY and the result of gettable

  if (udata != NULL) {
    return *static_cast<LuaObject**>(udata);
  }
  if (type == LUA_TFUNCTION) {
    return new LuaFunction(L, pos);
  }
  else if (type == LUA_TNUMBER) {
    return new LuaNumber(L, pos);
  }
  else if (type == LUA_TTABLE) {
    return new LuaTable(L, pos);
  }
  else {
    return new LuaObject(L, pos);
  }
}

static LuaFunction *thefunc = NULL;
static LuaNumber *thenumber = NULL;
static LuaTable *thetable = NULL;

int setfunc(lua_State *L)
{
  thefunc = dynamic_cast<LuaFunction*>(LuaObject::New(L, 1));
  if (!thefunc) {
    luaL_error(L, "object must be a function");
  }
  return 0;
}
int callfunc(lua_State *L)
{
  if (thefunc != NULL) {
    return thefunc->call();
  }
  else {
    return 0;
  }
}

int setnumber(lua_State *L)
{
  thenumber = dynamic_cast<LuaNumber*>(LuaObject::New(L, 1));
  if (!thenumber) {
    luaL_error(L, "object must be a number");
  }
  return 0;
}
int getnumber(lua_State *L)
{
  double val;
  if (thenumber != NULL) {
    val = thenumber->get_value();
  }
  else {
    val = 0.0;
  }
  lua_pushnumber(L, val);
  return 1;
}

int settable(lua_State *L)
{
  thetable = dynamic_cast<LuaTable*>(LuaObject::New(L, 1));
  if (!thetable) {
    luaL_error(L, "object must be a table");
  }
  return 0;
}
int gettable(lua_State *L)
{
  if (thetable != NULL) {
    thetable->push();
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

int main(int argc, char **argv)
{
  int n;
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  LuaObject::Init(L);
  luaL_Reg bridge[] = {{"setfunc", setfunc},
                       {"callfunc", callfunc},
                       {"setnumber", setnumber},
                       {"getnumber", getnumber},
                       {"settable", settable},
                       {"gettable", gettable},
                       {NULL, NULL}};
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  luaL_newlib(L, bridge);
  lua_setfield(L, -2, "bridge");
  lua_pop(L, 2);


  // Create the global `arg` table
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  for (n=0; n<argc; ++n) {
    lua_pushstring(L, argv[n]);
    lua_rawseti(L, -2, n);
  }
  lua_setglobal(L, "arg");


  // Run the script
  // ---------------------------------------------------------------------------
  if (argc == 1) {
    printf("usage: cow script.lua [arg1=val1 arg2=val2]\n");
  }
  else {
    if (luaL_dofile(L, argv[1])) {
      printf("%s\n", lua_tostring(L, -1));
    }
  }

  lua_close(L);
  return 0;
}
