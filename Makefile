
LUA_HOME = $(PWD)/lua-5.2.1

CFLAGS = -Wall

default : main

main : main.cpp lua_object.hpp
	$(CXX) $(CFLAGS) -o $@ $< -I$(LUA_HOME)/include $(LUA_HOME)/lib/liblua.a

clean :
	rm -f main
