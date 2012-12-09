
#ifndef __MYLIB_INCLUDE
#define __MYLIB_INCLUDE

struct my_struct
{
  int a;
  int b;
  double x;
  double y;
} ;

struct my_application
{
  struct my_struct cfg;
} ;

int func1();
int func2();

#endif // __MYLIB_INCLUDE
