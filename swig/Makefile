
SWIG = swig #/Library/Science/swig-2.0.8/bin/swig

LUA_A = ../lua-5.2.1/lib/liblua.a
LUA_I = -I../lua-5.2.1/include

default : main

mylib_wrap.c : mylib.h
	$(SWIG) -o $@ -lua -module mylib mylib.h
	patch -i mylib.patch $@

cpplib_wrap.cpp : cpplib.hpp
	$(SWIG) -o $@ -lua -module cpplib -c++ cpplib.hpp
	patch -i cpplib.patch $@

%.o : %.c
	gcc -c -o $@ $< $(LUA_I)

%.o : %.cpp
	g++ -c -o $@ $< $(LUA_I)

main : main.o mylib.o mylib_wrap.o cpplib.o cpplib_wrap.o
	g++ -Wall -o $@ $^ $(LUA_I) $(LUA_A)

clean :
	rm -f *.o *.orig main mylib_wrap.c cpplib_wrap.cpp
