
local function test_method_calls()
   local dog = tests.Dog()
   local cat = tests.Cat()
   for number, animal in pairs{dog, cat} do
      print("for animal type " .. animal:get_type())
      animal:speak()
      animal:eat(number)
   end
   local owner = tests.PetOwner()
   cat:set_name("orange")
   dog:set_name("murphy")
   owner:set_dog(dog)
   owner:set_cat(cat)
   assert("orange" == owner:get_cat():get_name())
end

local function test_casting()
   local dog = tests.Dog()
   local cat = tests.Cat()
   local jacko = tests.Poodle()
   local owner = tests.PetOwner()
   assert(dog:get_type() == "Dog")
   assert(cat:get_type() == "Cat")
   assert(jacko:get_type() == "Poodle")
   owner:set_cat(cat)
   owner:set_dog(jacko)
   assert(owner:get_cat() == cat)
   assert(owner:get_dog() == jacko)
end

local function test_gc()
   things = { }
   for i=1,10 do
      things[i] = tests.Dog()
   end
   print("should collect now...")
   things[1] = nil
   things[2] = nil
   things[3] = nil
   things[4] = nil
   collectgarbage()
   print("did it? ^^")
   things[1] = tests.Dog()
   things[2] = tests.Dog()
   things[3] = tests.Dog()
   things[4] = tests.Dog()
   print("collected?")
   for k,v in pairs(debug.getregistry()["__CXX_OBJECT_LOOKUP"]) do
      print(k,v)
   end
end

local function test_hold_drop()
   local david = tests.PetOwner()

   local sadie = tests.Dog()
   local murphy = tests.Dog()
   local david = tests.PetOwner()
   local laura = tests.PetOwner()

   david:auto_cat()
   david:set_dog(murphy)
   david:set_dog(sadie)
   sadie:set_owner(david)

   for k,v in pairs(getmetatable(david).__CXX_INSTANCE_HELD_OBJECTS) do
      print(k,v)
   end
   for k,v in pairs(getmetatable(sadie).__CXX_INSTANCE_HELD_OBJECTS) do
      print(k,v)
   end

   print("sadie, " .. sadie:get_refid() .. " should NOT be collected")
   print("murphy, " .. murphy:get_refid() .. " should be collected")

   murphy = nil
   sadie = nil

   collectgarbage()
   print("check what happened ^^")
end

local function test_callback()
   local sadie = tests.Dog()
   sadie:play()
   sadie:teach_play(function() print("chasing rabbit!") end)
   sadie:play()
   local cpp_func = tests.CppFunction()
   sadie:teach_play(cpp_func)
   cpp_func = nil
   collectgarbage()
   sadie:play()
end

local function test_add_method()
   local sadie = tests.Dog()
   function sadie:run_around()
      self:play()
      return "run around"
   end
   assert(sadie:run_around() == "run around")
end

local function test_complex()
   local z = tests.j + 2
   assert(z:get_re() == 2.0)
   assert(z:get_im() == 1.0)
end

local function test_lua_function()
   local f = tests.LuaFunction()
   f:set_function(function(x,y) return x,y,x*y end)
   local res1, res2, res3 = f(2,3)
   assert(res1 == 2)
   assert(res2 == 3)
   assert(res3 == 6)
end

test_complex()
test_add_method()
test_callback()
test_casting()
test_method_calls()
test_lua_function()

--test_hold_drop()
--test_gc()

--for k,v in pairs(debug.getregistry()) do print(k,v) end
