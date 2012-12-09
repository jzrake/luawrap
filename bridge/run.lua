
local bridge = require 'bridge'

local thefunc = function() return "in the func!" end
local thetable = {1,2,3}
local thenumber = 10.0
local Registry = debug.getregistry()
local Cpp2Lua = Registry["__LuaObject_CPP2LUA"] -- forward registry
local Lua2Cpp = Registry["__LuaObject_LUA2CPP"] -- reverse registry

local function TableSize(t)
   local n=0
   for k,v in pairs(t) do n = n + 1 end
   return n
end
local function TablePrint(t) for k,v in pairs(t) do print(k,v) end end


assert(getmetatable(Cpp2Lua).__mode == "kv") -- make sure weak tables/values
assert(getmetatable(Lua2Cpp).__mode == "kv")

TablePrint(Lua2Cpp)

assert(bridge.callfunc(thefunc) == "in the func!")
bridge.setitem(thetable, thenumber, thefunc)
assert(thetable[thenumber] == thefunc)
assert(bridge.getitem(thetable, thenumber) == thefunc)

assert(TableSize(Lua2Cpp) == 3)

print "all test passed"
