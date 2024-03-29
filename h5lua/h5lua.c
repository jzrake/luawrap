
#include <stdlib.h>
#include <string.h>
#include <hdf5.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


// -----------------------------------------------------------------------------
// hid_t
// -----------------------------------------------------------------------------
static void lh5_push_hid_t(lua_State *L, hid_t id)
{
  *((hid_t*) lua_newuserdata(L, sizeof(hid_t))) = id;
  luaL_setmetatable(L, "HDF5::hid_t");
}
static int h5lua_new_hid_t(lua_State *L)
{
  lh5_push_hid_t(L, 0);
  return 1;
}

// -----------------------------------------------------------------------------
// herr_t
// -----------------------------------------------------------------------------
static void lh5_push_herr_t(lua_State *L, herr_t id)
{
  *((herr_t*) lua_newuserdata(L, sizeof(herr_t))) = id;
  luaL_setmetatable(L, "HDF5::herr_t");
}
static int h5lua_new_herr_t(lua_State *L)
{
  lh5_push_herr_t(L, 0);
  return 1;
}

// -----------------------------------------------------------------------------
// H5O_info_t
// -----------------------------------------------------------------------------
static void lh5_push_H5O_info_t(lua_State *L, H5O_info_t id)
{
  *((H5O_info_t*) lua_newuserdata(L, sizeof(H5O_info_t))) = id;
  luaL_setmetatable(L, "HDF5::H5O_info_t");
}
static int h5lua_new_H5O_info_t(lua_State *L)
{
  H5O_info_t id;
  lh5_push_H5O_info_t(L, id);
  return 1;
}
static int h5lua_H5O_info_t__index(lua_State *L)
{
  H5O_info_t *i = (H5O_info_t*) luaL_checkudata(L, 1, "HDF5::H5O_info_t");
  const char *key = luaL_checkstring(L, 2);
  if (0) { }
  else if (strcmp(key, "fileno") == 0) { lua_pushnumber(L, i->fileno); }
  else if (strcmp(key, "addr") == 0) { lua_pushnumber(L, i->addr); }
  else if (strcmp(key, "type") == 0) { lua_pushnumber(L, i->type); }
  else if (strcmp(key, "rc") == 0) { lua_pushnumber(L, i->rc); }
  else if (strcmp(key, "atime") == 0) { lua_pushnumber(L, i->atime); }
  else if (strcmp(key, "mtime") == 0) { lua_pushnumber(L, i->mtime); }
  else if (strcmp(key, "ctime") == 0) { lua_pushnumber(L, i->ctime); }
  else if (strcmp(key, "btime") == 0) { lua_pushnumber(L, i->btime); }
  else if (strcmp(key, "num_attrs") == 0) { lua_pushnumber(L, i->num_attrs); }
  else { lua_pushnil(L); }
  return 1;
}
static int h5lua_H5O_info_t__newindex(lua_State *L)
{
  luaL_checkudata(L, 1, "HDF5::H5O_info_t");
  luaL_error(L, "object does not support item assignment");
  return 0;
}

// -----------------------------------------------------------------------------
// hsize_t_arr
// -----------------------------------------------------------------------------
static void lh5_push_hsize_t_arr(lua_State *L, hsize_t *hs, unsigned int N)
{
  memcpy(lua_newuserdata(L, sizeof(hsize_t) * N), hs, sizeof(hsize_t) * N);
  luaL_setmetatable(L, "HDF5::hsize_t_arr");
}
static int h5lua_new_hsize_t_arr(lua_State *L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  int n = 0, N = lua_rawlen(L, 1);
  hsize_t *hs = (hsize_t*) malloc(sizeof(hsize_t) * N);

  while (n < N) {
    lua_rawgeti(L, 1, n+1);
    hs[n] = lua_tounsigned(L, -1);
    lua_pop(L, 1);
    ++n;
  }

  lh5_push_hsize_t_arr(L, hs, N);
  free(hs);
  return 1;
}
static int h5lua_hsize_t_arr__index(lua_State *L)
{
  hsize_t *lhs = (hsize_t*) luaL_checkudata(L, 1, "HDF5::hsize_t_arr");
  unsigned int n = luaL_checkunsigned(L, 2);
  unsigned int N = lua_rawlen(L, 1) / sizeof(hsize_t);
  if (n < N) {
    lua_pushnumber(L, lhs[n]);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}
static int h5lua_hsize_t_arr__newindex(lua_State *L)
{
  hsize_t *lhs = (hsize_t*) luaL_checkudata(L, 1, "HDF5::hsize_t_arr");
  unsigned int n = luaL_checkunsigned(L, 2);
  hsize_t val = luaL_checkunsigned(L, 3);
  unsigned int N = lua_rawlen(L, 1) / sizeof(hsize_t);
  if (n < N) {
    lhs[n] = val;
  }
  else {
    luaL_error(L, "index %d out of range on array of length %d", n, N);
  }
  return 1;
}





// -----------------------------------------------------------------------------
// double_arr
// -----------------------------------------------------------------------------
static void lh5_push_double_arr(lua_State *L, double *hs, unsigned int N)
{
  memcpy(lua_newuserdata(L, sizeof(double) * N), hs, sizeof(double) * N);
  luaL_setmetatable(L, "HDF5::double_arr");
}
static int h5lua_new_double_arr(lua_State *L)
{
  unsigned int n = 0, N = luaL_checkunsigned(L, 1);
  double *hs = (double*) malloc(sizeof(double) * N);
  while (n < N) {
    hs[n] = 0.0;    
    ++n;
  }
  lh5_push_double_arr(L, hs, N);
  free(hs);
  return 1;
}
static int h5lua_double_arr__index(lua_State *L)
{
  double *lhs = (double*) luaL_checkudata(L, 1, "HDF5::double_arr");
  unsigned int n = luaL_checkunsigned(L, 2);
  unsigned int N = lua_rawlen(L, 1) / sizeof(double);
  if (n < N) {
    lua_pushnumber(L, lhs[n]);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}
static int h5lua_double_arr__newindex(lua_State *L)
{
  double *lhs = (double*) luaL_checkudata(L, 1, "HDF5::double_arr");
  unsigned int n = luaL_checkunsigned(L, 2);
  double val = luaL_checknumber(L, 3);
  unsigned int N = lua_rawlen(L, 1) / sizeof(double);
  if (n < N) {
    lhs[n] = val;
  }
  else {
    luaL_error(L, "index %d out of range on array of length %d", n, N);
  }
  return 1;
}





// -----------------------------------------------------------------------------
// By-hand wrappers
// -----------------------------------------------------------------------------
static herr_t _H5Literate_cb(hid_t g_id, const char *name,
			     const H5L_info_t *info, void *op_data)
{
  /*
   *   function expects
   *
   * + op_data is a lua_State*
   * + function on top of Lua stack
   */
  lua_State *L = (lua_State*) op_data;
  lua_pushvalue(L, -1);
  lua_pushstring(L, name);
  lua_call(L, 1, 0);
  return 0;
}
int h5lua_H5Literate(lua_State *L)
{
  hid_t group_id = *((hid_t*) luaL_checkudata(L, 1, "HDF5::hid_t"));
  H5_index_t index_type = luaL_checkinteger(L, 2);
  H5_iter_order_t order = luaL_checkinteger(L, 3);
  hsize_t *idx = (hsize_t*) luaL_checkudata(L, 4, "HDF5::hsize_t_arr");
  luaL_checktype(L, 5, LUA_TFUNCTION);
  H5L_iterate_t op = _H5Literate_cb;
  int ret = H5Literate(group_id, index_type, order, idx, op, L);
  return ret;
}


// -----------------------------------------------------------------------------
// Python-generated wrappers
// -----------------------------------------------------------------------------
#include "h5funcs.c"


int luaopen_h5lua(lua_State *L)
{
  luaL_Reg h5lua_types[] = {
    {"new_hid_t", h5lua_new_hid_t},
    {"new_herr_t", h5lua_new_herr_t},
    {"new_H5O_info_t", h5lua_new_H5O_info_t},
    {"new_double_arr", h5lua_new_double_arr},
    {"new_hsize_t_arr", h5lua_new_hsize_t_arr},
    {NULL, NULL}};

  luaL_Reg H5O_info_t_meta[] = {
    {"__index", h5lua_H5O_info_t__index},
    {"__newindex", h5lua_H5O_info_t__newindex},
    {NULL, NULL}};
  luaL_newmetatable(L, "HDF5::H5O_info_t");
  luaL_setfuncs(L, H5O_info_t_meta, 0);
  lua_pop(L, 1);

  luaL_Reg hsize_t_arr_meta[] = {
    {"__index", h5lua_hsize_t_arr__index},
    {"__newindex", h5lua_hsize_t_arr__newindex},
    {NULL, NULL}};
  luaL_newmetatable(L, "HDF5::hsize_t_arr");
  luaL_setfuncs(L, hsize_t_arr_meta, 0);
  lua_pop(L, 1);

  luaL_Reg double_arr_meta[] = {
    {"__index", h5lua_double_arr__index},
    {"__newindex", h5lua_double_arr__newindex},
    {NULL, NULL}};
  luaL_newmetatable(L, "HDF5::double_arr");
  luaL_setfuncs(L, double_arr_meta, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, "HDF5::hid_t");
  lua_pop(L, 1);
  luaL_newmetatable(L, "HDF5::herr_t");
  lua_pop(L, 1);


  // ---------------------------------------------------------------------------
  // Module definition
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  luaL_setfuncs(L, h5lua_types, 0);
  luaL_setfuncs(L, H5A_funcs, 0);
  luaL_setfuncs(L, H5D_funcs, 0);
  luaL_setfuncs(L, H5E_funcs, 0);
  luaL_setfuncs(L, H5F_funcs, 0);
  luaL_setfuncs(L, H5G_funcs, 0);
  luaL_setfuncs(L, H5I_funcs, 0);
  luaL_setfuncs(L, H5L_funcs, 0);
  luaL_setfuncs(L, H5O_funcs, 0);
  luaL_setfuncs(L, H5P_funcs, 0);
  luaL_setfuncs(L, H5R_funcs, 0);
  luaL_setfuncs(L, H5S_funcs, 0);
  luaL_setfuncs(L, H5T_funcs, 0);
  luaL_setfuncs(L, H5Z_funcs, 0);

#define REG_NUMBER(s) lua_pushnumber(L, s); lua_setfield(L, -2, #s);
  REG_NUMBER(H5F_ACC_RDONLY);
  REG_NUMBER(H5F_ACC_RDWR);
  REG_NUMBER(H5F_ACC_TRUNC);
  REG_NUMBER(H5F_ACC_EXCL);
  REG_NUMBER(H5F_ACC_DEBUG);
  REG_NUMBER(H5F_ACC_CREAT);
  REG_NUMBER(H5F_ACC_DEFAULT);
  REG_NUMBER(H5F_OBJ_FILE);
  REG_NUMBER(H5F_OBJ_DATASET);
  REG_NUMBER(H5F_OBJ_GROUP);
  REG_NUMBER(H5F_OBJ_DATATYPE);
  REG_NUMBER(H5F_OBJ_ATTR);
  REG_NUMBER(H5F_OBJ_ALL);
  REG_NUMBER(H5F_OBJ_LOCAL);
  REG_NUMBER(H5F_FAMILY_DEFAULT);
  REG_NUMBER(H5F_UNLIMITED);
  REG_NUMBER(H5S_NO_CLASS);
  REG_NUMBER(H5S_SCALAR);
  REG_NUMBER(H5S_SIMPLE);
  REG_NUMBER(H5S_NULL);
  REG_NUMBER(H5S_SELECT_NOOP);
  REG_NUMBER(H5S_SELECT_SET);
  REG_NUMBER(H5S_SELECT_OR);
  REG_NUMBER(H5S_SELECT_AND);
  REG_NUMBER(H5S_SELECT_XOR);
  REG_NUMBER(H5S_SELECT_NOTB);
  REG_NUMBER(H5S_SELECT_NOTA);
  REG_NUMBER(H5S_SELECT_APPEND);
  REG_NUMBER(H5S_SELECT_PREPEND);
  REG_NUMBER(H5S_SELECT_INVALID);
  REG_NUMBER(H5S_SEL_ERROR);
  REG_NUMBER(H5S_SEL_NONE);
  REG_NUMBER(H5S_SEL_POINTS);
  REG_NUMBER(H5S_SEL_HYPERSLABS);
  REG_NUMBER(H5S_SEL_ALL);
  REG_NUMBER(H5S_SEL_N);
  REG_NUMBER(H5O_TYPE_UNKNOWN);
  REG_NUMBER(H5O_TYPE_GROUP);
  REG_NUMBER(H5O_TYPE_DATASET);
  REG_NUMBER(H5O_TYPE_NAMED_DATATYPE);
  REG_NUMBER(H5O_TYPE_NTYPES);
  REG_NUMBER(H5L_TYPE_ERROR);
  REG_NUMBER(H5L_TYPE_HARD);
  REG_NUMBER(H5L_TYPE_SOFT);
  REG_NUMBER(H5L_TYPE_EXTERNAL);
  REG_NUMBER(H5L_TYPE_MAX);
  REG_NUMBER(H5_VERS_RELEASE);
  REG_NUMBER(H5_ITER_UNKNOWN);
  REG_NUMBER(H5_ITER_INC);
  REG_NUMBER(H5_ITER_DEC);
  REG_NUMBER(H5_ITER_NATIVE);
  REG_NUMBER(H5_ITER_N);
  REG_NUMBER(H5_INDEX_UNKNOWN);
  REG_NUMBER(H5_INDEX_NAME);
  REG_NUMBER(H5_INDEX_CRT_ORDER);
  REG_NUMBER(H5_INDEX_N);
#undef REG_NUMBER

#define REG_HID(s) lh5_push_hid_t(L, s); lua_setfield(L, -2, #s);
  REG_HID(H5P_DEFAULT);
  REG_HID(H5P_ROOT);
  REG_HID(H5P_OBJECT_CREATE);
  REG_HID(H5P_FILE_CREATE);
  REG_HID(H5P_FILE_ACCESS);
  REG_HID(H5P_DATASET_CREATE);
  REG_HID(H5P_DATASET_ACCESS);
  REG_HID(H5P_DATASET_XFER);
  REG_HID(H5P_FILE_MOUNT);
  REG_HID(H5P_GROUP_CREATE);
  REG_HID(H5P_GROUP_ACCESS);
  REG_HID(H5P_DATATYPE_CREATE);
  REG_HID(H5P_DATATYPE_ACCESS);
  REG_HID(H5P_STRING_CREATE);
  REG_HID(H5P_ATTRIBUTE_CREATE);
  REG_HID(H5P_OBJECT_COPY);
  REG_HID(H5P_LINK_CREATE);
  REG_HID(H5P_LINK_ACCESS);
  REG_HID(H5P_FILE_CREATE_DEFAULT);
  REG_HID(H5P_FILE_ACCESS_DEFAULT);
  REG_HID(H5P_DATASET_CREATE_DEFAULT);
  REG_HID(H5P_DATASET_ACCESS_DEFAULT);
  REG_HID(H5P_DATASET_XFER_DEFAULT);
  REG_HID(H5P_FILE_MOUNT_DEFAULT);
  REG_HID(H5P_GROUP_CREATE_DEFAULT);
  REG_HID(H5P_GROUP_ACCESS_DEFAULT);
  REG_HID(H5P_DATATYPE_CREATE_DEFAULT);
  REG_HID(H5P_DATATYPE_ACCESS_DEFAULT);
  REG_HID(H5P_ATTRIBUTE_CREATE_DEFAULT);
  REG_HID(H5P_OBJECT_COPY_DEFAULT);
  REG_HID(H5P_LINK_CREATE_DEFAULT);
  REG_HID(H5P_LINK_ACCESS_DEFAULT);
  REG_HID(H5P_CRT_ORDER_TRACKED);
  REG_HID(H5P_CRT_ORDER_INDEXED);
  REG_HID(H5P_NO_CLASS);
  REG_HID(H5T_NCSET);
  REG_HID(H5T_NSTR);
  REG_HID(H5T_VARIABLE);
  REG_HID(H5T_OPAQUE_TAG_MAX);
  REG_HID(H5T_IEEE_F32BE);
  REG_HID(H5T_IEEE_F32LE);
  REG_HID(H5T_IEEE_F64BE);
  REG_HID(H5T_IEEE_F64LE);
  REG_HID(H5T_STD_I8BE);
  REG_HID(H5T_STD_I8LE);
  REG_HID(H5T_STD_I16BE);
  REG_HID(H5T_STD_I16LE);
  REG_HID(H5T_STD_I32BE);
  REG_HID(H5T_STD_I32LE);
  REG_HID(H5T_STD_I64BE);
  REG_HID(H5T_STD_I64LE);
  REG_HID(H5T_STD_U8BE);
  REG_HID(H5T_STD_U8LE);
  REG_HID(H5T_STD_U16BE);
  REG_HID(H5T_STD_U16LE);
  REG_HID(H5T_STD_U32BE);
  REG_HID(H5T_STD_U32LE);
  REG_HID(H5T_STD_U64BE);
  REG_HID(H5T_STD_U64LE);
  REG_HID(H5T_STD_B8BE);
  REG_HID(H5T_STD_B8LE);
  REG_HID(H5T_STD_B16BE);
  REG_HID(H5T_STD_B16LE);
  REG_HID(H5T_STD_B32BE);
  REG_HID(H5T_STD_B32LE);
  REG_HID(H5T_STD_B64BE);
  REG_HID(H5T_STD_B64LE);
  REG_HID(H5T_STD_REF_OBJ);
  REG_HID(H5T_STD_REF_DSETREG);
  REG_HID(H5T_UNIX_D32BE);
  REG_HID(H5T_UNIX_D32LE);
  REG_HID(H5T_UNIX_D64BE);
  REG_HID(H5T_UNIX_D64LE);
  REG_HID(H5T_C_S1);
  REG_HID(H5T_FORTRAN_S1);
  REG_HID(H5T_INTEL_I8);
  REG_HID(H5T_INTEL_I16);
  REG_HID(H5T_INTEL_I32);
  REG_HID(H5T_INTEL_I64);
  REG_HID(H5T_INTEL_U8);
  REG_HID(H5T_INTEL_U16);
  REG_HID(H5T_INTEL_U32);
  REG_HID(H5T_INTEL_U64);
  REG_HID(H5T_INTEL_B8);
  REG_HID(H5T_INTEL_B16);
  REG_HID(H5T_INTEL_B32);
  REG_HID(H5T_INTEL_B64);
  REG_HID(H5T_INTEL_F32);
  REG_HID(H5T_INTEL_F64);
  REG_HID(H5T_ALPHA_I8);
  REG_HID(H5T_ALPHA_I16);
  REG_HID(H5T_ALPHA_I32);
  REG_HID(H5T_ALPHA_I64);
  REG_HID(H5T_ALPHA_U8);
  REG_HID(H5T_ALPHA_U16);
  REG_HID(H5T_ALPHA_U32);
  REG_HID(H5T_ALPHA_U64);
  REG_HID(H5T_ALPHA_B8);
  REG_HID(H5T_ALPHA_B16);
  REG_HID(H5T_ALPHA_B32);
  REG_HID(H5T_ALPHA_B64);
  REG_HID(H5T_ALPHA_F32);
  REG_HID(H5T_ALPHA_F64);
  REG_HID(H5T_MIPS_I8);
  REG_HID(H5T_MIPS_I16);
  REG_HID(H5T_MIPS_I32);
  REG_HID(H5T_MIPS_I64);
  REG_HID(H5T_MIPS_U8);
  REG_HID(H5T_MIPS_U16);
  REG_HID(H5T_MIPS_U32);
  REG_HID(H5T_MIPS_U64);
  REG_HID(H5T_MIPS_B8);
  REG_HID(H5T_MIPS_B16);
  REG_HID(H5T_MIPS_B32);
  REG_HID(H5T_MIPS_B64);
  REG_HID(H5T_MIPS_F32);
  REG_HID(H5T_MIPS_F64);
  REG_HID(H5T_VAX_F32);
  REG_HID(H5T_VAX_F64);
  REG_HID(H5T_NATIVE_CHAR);
  REG_HID(H5T_NATIVE_SCHAR);
  REG_HID(H5T_NATIVE_UCHAR);
  REG_HID(H5T_NATIVE_SHORT);
  REG_HID(H5T_NATIVE_USHORT);
  REG_HID(H5T_NATIVE_INT);
  REG_HID(H5T_NATIVE_UINT);
  REG_HID(H5T_NATIVE_LONG);
  REG_HID(H5T_NATIVE_ULONG);
  REG_HID(H5T_NATIVE_LLONG);
  REG_HID(H5T_NATIVE_ULLONG);
  REG_HID(H5T_NATIVE_FLOAT);
  REG_HID(H5T_NATIVE_DOUBLE);
  REG_HID(H5T_NATIVE_LDOUBLE);
  REG_HID(H5T_NATIVE_B8);
  REG_HID(H5T_NATIVE_B16);
  REG_HID(H5T_NATIVE_B32);
  REG_HID(H5T_NATIVE_B64);
  REG_HID(H5T_NATIVE_OPAQUE);
  REG_HID(H5T_NATIVE_HADDR);
  REG_HID(H5T_NATIVE_HSIZE);
  REG_HID(H5T_NATIVE_HSSIZE);
  REG_HID(H5T_NATIVE_HERR);
  REG_HID(H5T_NATIVE_HBOOL);
  REG_HID(H5T_NATIVE_INT8);
  REG_HID(H5T_NATIVE_UINT8);
  REG_HID(H5T_NATIVE_INT_LEAST8);
  REG_HID(H5T_NATIVE_UINT_LEAST8);
  REG_HID(H5T_NATIVE_INT_FAST8);
  REG_HID(H5T_NATIVE_UINT_FAST8);
  REG_HID(H5T_NATIVE_INT16);
  REG_HID(H5T_NATIVE_UINT16);
  REG_HID(H5T_NATIVE_INT_LEAST16);
  REG_HID(H5T_NATIVE_UINT_LEAST16);
  REG_HID(H5T_NATIVE_INT_FAST16);
  REG_HID(H5T_NATIVE_UINT_FAST16);
  REG_HID(H5T_NATIVE_INT32);
  REG_HID(H5T_NATIVE_UINT32);
  REG_HID(H5T_NATIVE_INT_LEAST32);
  REG_HID(H5T_NATIVE_UINT_LEAST32);
  REG_HID(H5T_NATIVE_INT_FAST32);
  REG_HID(H5T_NATIVE_UINT_FAST32);
  REG_HID(H5T_NATIVE_INT64);
  REG_HID(H5T_NATIVE_UINT64);
  REG_HID(H5T_NATIVE_INT_LEAST64);
  REG_HID(H5T_NATIVE_UINT_LEAST64);
  REG_HID(H5T_NATIVE_INT_FAST64);
  REG_HID(H5T_NATIVE_UINT_FAST64);
#undef RED_HID

  return 1;
}
