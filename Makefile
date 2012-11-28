
LUA_HOME = $(HOME)/Work/luview/dep

CFLAGS = -Wall

default : main

main : main.cpp lua_object.hpp
	$(CXX) $(CFLAGS) -o $@ $< -I$(LUA_HOME)/include $(LUA_HOME)/lib/liblua.a

clean :
	rm -f main
