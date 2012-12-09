
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

local function TablePrint(t)
   for k,v in pairs(t) do print(k,v) end
end

assert(getmetatable(Cpp2Lua).__mode == "kv") -- make sure weak tables/values
assert(getmetatable(Lua2Cpp).__mode == "kv")

bridge.setfunc(thefunc)
bridge.settable(thetable)
bridge.setnumber(thenumber)

assert(bridge.callfunc() == "in the func!")
assert(bridge.getnumber() == 10.0)
assert(bridge.gettable()[1] == 1)

assert(type(Lua2Cpp[thefunc] == "userdata"))
assert(type(Lua2Cpp[thetable] == "userdata"))
assert(type(Lua2Cpp[thenumber] == "userdata"))

bridge.setitem(thetable, thenumber, thefunc)
assert(thetable[thenumber] == thefunc)
assert(bridge.getitem(thetable, thenumber) == thefunc)

--TablePrint(thetable)
--TablePrint(Lua2Cpp)

assert(TableSize(Lua2Cpp) == 3)

local thefunc = nil
local thetable = nil
local thenumber = nil

collectgarbage()
assert(TableSize(Lua2Cpp) == 0)

print "all test passed"




--bridge.testtable(thetable)
--for k,v in pairs(Cpp2Lua) do print(k,v) end
--for k,v in pairs(Lua2Cpp) do print(k,v) end
