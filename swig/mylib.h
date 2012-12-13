
#ifndef __MYLIB_INCLUDE
#define __MYLIB_INCLUDE

struct vector4
{
  double x, y, z, w;
} ;
struct my_struct
{
  int a;
  int b;
  double x;
  double y;
  struct vector4 vec;
} ;

struct my_application
{
  struct my_struct cfg;
} ;

int func1();
int func2();

#endif // __MYLIB_INCLUDE
