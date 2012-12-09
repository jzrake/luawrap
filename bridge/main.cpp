
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


#define LuaObject_LUA2CPP    "__LuaObject_LUA2CPP"
#define LuaObject_CPP2LUA    "__LuaObject_CPP2LUA"
#define LuaObject_METATABLE  "__LuaObject_METATABLE"


/*
 *
 * Forward registry:
 * -----------------
 * LuaObject_CPP2LUA  = { C++ LuaObject[lightuserdata]: [pure Lua object] }
 *
 * [+] used to push the pure Lua object onto the stack, given its C++ wrapper
 *
 * [+] only one C++ LuaObject per pure Lua object, ensured by LuaObject::New
 *
 *
 * Reverse registry:
 * -----------------
 * LuaObject_LUA2CPP = { [pure Lua object]: C++ LuaObject[fulluserdata] }
 *
 * [+] has weak keys and values
 *
 * [+] used to check for an existing LuaObject associated to the pure Lua object
 *
 *
 *
 * There are 3 objects going around:
 *
 * 1. pure Lua object: the primary lifeline of the other two
 * 2. C++ LuaObject instance pointer: a lightuserdata
 * 3. C++ LuaObject fulluserdata: controls deletion of (2) through its metatable
 *
 *
 * The primary object is the pure Lua object, which must be held in an external
 * table or somewhere on the stack in order not to be collected. The forward
 * registry (CPP2LUA) associates the C++ LuaObject instance pointer (as a
 * lightuserdata) with the pure Lua object. The forward registry has both weak
 * keys and values, so that the C++ LuaObject instance pointer (the key) will be
 * collected along with the pure Lua object, as soon as all outside references
 * to the pure Lua object vanish.
 *
 * The C++ instance is secondary, and is never a strong key in any table.
 * Instead, it will be kept alive only as long as its associated pure Lua object
 * is referenced from an outside table or the stack. The reverse registry
 * (LUA2CPP) associates the pure Lua object with the C++ LuaObject fulluserdata,
 * and also maintains both weak keys and values.
 *
 * Newly created C++ instances wrapping pure Lua objects need to be kept on the
 * stack until they are assigned to some other table, otherwise they are subject
 * to garbage collection.
 *
 */

class LuaObject
{
protected:
  lua_State *L;
private:
  LuaObject(const LuaObject &other) { } // copy constructor is illegal
public:
  static void Init(lua_State *L)
  {
    lua_newtable(L); // create the LuaObject_CPP2LUA
    lua_newtable(L); // create its metatable
    lua_pushstring(L, "kv"); // LuaObject_CPP2LUA needs weak keys and values:
    lua_setfield(L, -2, "__mode"); // meta(LuaObject_CPP2LUA)[__mode] = "kv"
    lua_setmetatable(L, -2); // assign to the LuaObject_CPP2LUA's metatable
    lua_setfield(L, LUA_REGISTRYINDEX, LuaObject_CPP2LUA); // stack now empty

    lua_newtable(L); // create the LuaObject_LUA2CPP
    lua_newtable(L); // create its metatable
    lua_pushstring(L, "kv"); // LuaObject_LUA2CPP needs weak keys and values:
    lua_setfield(L, -2, "__mode"); // meta(LuaObject_LUA2CPP)[__mode] = "kv"
    lua_setmetatable(L, -2); // assign to the LuaObject_LUA2CPP's metatable
    lua_setfield(L, LUA_REGISTRYINDEX, LuaObject_LUA2CPP); // stack now empty

    luaL_newmetatable(L, LuaObject_METATABLE);
    lua_pushcfunction(L, __gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1); // stack now empty
  }
  LuaObject(lua_State *L, int pos) : L(L)
  {
    int isnil = (lua_type(L, pos) == LUA_TNIL);
    if (isnil) {
      lua_pushlightuserdata(L, NULL);
    }
    else {
      lua_pushvalue(L, pos);
    }

    lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_CPP2LUA);
    lua_pushvalue(L, -2); // pushes the pure Lua object
    lua_rawsetp(L, -2, this); // pops the pure Lua object
    lua_pop(L, 1); // remove the LuaObject_CPP2LUA table: (+1, pure Lua value)


    lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_LUA2CPP);
    if (isnil) {
      lua_pushlightuserdata(L, NULL); // nil cannot be used as an index
    }
    else {
      lua_pushvalue(L, -2); // pushes the pure Lua object: key
    }
    *((LuaObject**) lua_newuserdata(L, sizeof(LuaObject*))) = this;
    luaL_setmetatable(L, LuaObject_METATABLE); // does not change stack
    lua_settable(L, -3);
    lua_pop(L, 1); // remove the LuaObject_LUA2CPP table: (+1, pure Lua value)

    lua_pop(L, 1); // remove the pure Lua value: clean stack
  }
  virtual ~LuaObject() { }

  void push()
  {
    lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_CPP2LUA);
    lua_rawgetp(L, -1, this);
    lua_remove(L, -2);
  }
  int type()
  {
    this->push();
    int t = lua_type(L, -1);
    lua_pop(L, 1);
    return t;
  }
  static LuaObject *New(lua_State *L, int pos);
  template<class T> static T *New(lua_State *L, int pos);
  static int __gc(lua_State *L)
  {
    LuaObject *self = *static_cast<LuaObject**>(lua_touserdata(L, -1));
    delete self;
    return 0;
  }
} ;

class LuaFunction : public LuaObject
{
private:
  LuaFunction(const LuaFunction &other) : LuaObject(NULL, 0) { }
  LuaFunction &operator=(const LuaFunction &other) { return *this; }
public:
  static const int type_id = LUA_TFUNCTION;
  LuaFunction(lua_State *L, int pos) : LuaObject(L, pos) { }
  int call()
  {
    int start = lua_gettop(L);
    this->push();
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L) - start;
  }
} ;

class LuaNumber : public LuaObject
{
private:
  LuaNumber(const LuaNumber &other) : LuaObject(NULL, 0) { }
  LuaNumber &operator=(const LuaNumber &other) { return *this; }
public:
  static const int type_id = LUA_TNUMBER;
  LuaNumber(lua_State *L, int pos) : LuaObject(L, pos) { }
  LuaObject &operator=(const double x)
  {
    lua_pushnumber(L, x);
    lua_rawsetp(L, LUA_REGISTRYINDEX, this);
    return *this;
  }
  double get_value()
  {
    double ret;
    this->push();
    ret = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ret;
  }
} ;

class LuaTable : public LuaObject
{
private:
  LuaTable(const LuaTable &other) : LuaObject(NULL, 0) { }
  LuaTable &operator=(const LuaTable &other) { return *this; }
public:
  static const int type_id = LUA_TTABLE;
  LuaTable(lua_State *L, int pos) : LuaObject(L, pos) { }
  void set(LuaObject *key, LuaObject *val)
  {
    this->push();
    key->push();
    val->push();
    lua_settable(L, -3);
    lua_pop(L, 1);
  }
  LuaObject *get(LuaObject *key)
  {
    this->push();
    key->push();
    lua_gettable(L, -2);
    LuaObject *ret = New(L, -1);
    lua_pop(L, 1);
    return ret;
  }
  /*
  LuaObject &operator[](const double x)
  {
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    lua_pushnumber(L, x);
    lua_gettable(L, -2);
    LuaObject *ret = New(L, -1);
    lua_pop(L, 1);
    return *ret;
  }
  LuaObject &operator[](const LuaObject *key)
  {
    lua_rawgetp(L, LUA_REGISTRYINDEX, this);
    lua_rawgetp(L, LUA_REGISTRYINDEX, key);
    lua_gettable(L, -2);
    LuaObject *ret = New(L, -1);
    lua_pop(L, 1);
    return *ret;
  }
  */
} ;


LuaObject *LuaObject::New(lua_State *L, int pos)
{
  pos = lua_absindex(L, pos);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject_LUA2CPP);
  lua_pushvalue(L, pos);
  lua_gettable(L, -2);

  void *udata = lua_touserdata(L, -1);
  int type = lua_type(L, pos);

  lua_pop(L, 2); // remove LuaObject_LUA2CPP and the result of gettable

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

template <class T> T *LuaObject::New(lua_State *L, int pos)
{
  LuaObject *obj = LuaObject::New(L, pos);
  int type_need = T::type_id;
  int type_have = lua_type(L, pos);

  if (type_need != type_have) {
    const char *stype_need = lua_typename(L, type_need);
    const char *stype_have = lua_typename(L, type_have);
    luaL_error(L, "TypeError: expected %s, got %s\n", stype_need, stype_have);
  }
  return dynamic_cast<T*>(obj);
}

static LuaFunction *thefunc = NULL;
static LuaNumber *thenumber = NULL;
static LuaTable *thetable = NULL;


int getfunc(lua_State *L)
{
  if (thefunc != NULL) {
    thefunc->push();
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}
int setfunc(lua_State *L)
{
  thefunc = LuaObject::New<LuaFunction>(L, 1);
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
int setnumber(lua_State *L)
{
  thenumber = LuaObject::New<LuaNumber>(L, 1);
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
int settable(lua_State *L)
{
  thetable = LuaObject::New<LuaTable>(L, 1);
  return 0;
}

int getitem(lua_State *L)
{
  LuaTable *table = LuaObject::New<LuaTable>(L, 1);
  LuaObject *key = LuaObject::New(L, 2);
  LuaObject *val = table->get(key);
  val->push();
  return 1;
}
int setitem(lua_State *L)
{
  LuaTable *table = LuaObject::New<LuaTable>(L, 1);
  LuaObject *key = LuaObject::New(L, 2);
  LuaObject *val = LuaObject::New(L, 3);
  table->set(key, val);
  return 0;
}

int testtable(lua_State *L)
{
  //  LuaTable *table = LuaObject::New<LuaTable>(L, 1);
  //  lua_newtable(L);
  //  LuaTable &subtable = *LuaObject::New<LuaTable>(L, -1);
  //  table->operator[](100);
  return 0;
}

int main(int argc, char **argv)
{
  int n;
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  LuaObject::Init(L);
  luaL_Reg bridge[] = {{"getfunc", getfunc},
                       {"setfunc", setfunc},
                       {"callfunc", callfunc},
                       {"getnumber", getnumber},
                       {"setnumber", setnumber},
                       {"gettable", gettable},
                       {"settable", settable},
                       {"getitem", getitem},
                       {"setitem", setitem},
                       {"testtable", testtable},
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
