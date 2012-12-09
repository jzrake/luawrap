
LUA_HOME = ../lua-5.2.1

CFLAGS = -Wall

default : main

main : main.cpp
	$(CXX) $(CFLAGS) -o $@ $< -I$(LUA_HOME)/include $(LUA_HOME)/lib/liblua.a

clean :
	rm -f main
