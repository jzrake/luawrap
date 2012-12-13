
local mylib = require 'mylib'
local cpplib = require 'cpplib'

assert(mylib.func1() == 1)
assert(mylib.func2() == 2)

local s = mylib.my_struct()
local A = mylib.my_application()
assert(getmetatable(s) ~= nil)
assert(getmetatable(A) ~= nil)

s.a = 3.2
s.b = 2
s.x = 2.1
s.y = 2
s.vec.x = 10.0
s.vec.y = 20.0
s.vec.z = 30.0
s.vec.w = 40.0

assert(s.a == 3)
assert(s.b == 2)
assert(s.x == 2.1)
assert(s.y == 2.0)

assert(s.vec.x == 10)
assert(s.vec.y == 20)
assert(s.vec.z == 30)
assert(s.vec.w == 40)

A.cfg = s
local s = nil
collectgarbage()

assert(A.cfg.a == 3)
assert(A.cfg.b == 2)
assert(A.cfg.x == 2.1)
assert(A.cfg.y == 2.0)

local L = cpplib.Line()
assert(getmetatable(L) ~= nil)

L.p1 = cpplib.Point(1,2)
L.p2 = cpplib.Point(3,4)

assert(L.p1.x == 1)
assert(L.p1.y == 2)
assert(L.p2.x == 3)
assert(L.p2.y == 4)


print "all tests passed!"


