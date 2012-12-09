
local bridge = require 'bridge'

local thefunc = function() return "in the func!" end
local thetable = {1,2,3}
local thenumber = 10.0
local Registry = debug.getregistry()
local LuaObject = Registry["__LuaObject_REGISTRY"]

bridge.setfunc(thefunc)
bridge.settable(thetable)
bridge.setnumber(thenumber)

assert(bridge.callfunc() == "in the func!")
assert(bridge.getnumber() == 10.0)
assert(bridge.gettable()[1] == 1)

assert(getmetatable(LuaObject).__mode == "v")
assert(type(LuaObject[thefunc] == "userdata"))
assert(type(LuaObject[thetable] == "userdata"))
assert(type(LuaObject[thenumber] == "userdata"))

bridge.setitem(thetable, thenumber, thefunc)
assert(thetable[thenumber] == thefunc)
assert(bridge.getitem(thetable, thenumber) == thefunc)

--print(bridge.getitem(thetable, thenumber))
--for k,v in pairs(Registry) do print(k,v) end

print "all test passed"
