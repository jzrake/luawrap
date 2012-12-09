
#include <cstdio>
#include "cpplib.hpp"

Point::Point(double x, double y) : x(x), y(y) { }
Line::Line() : p1(0,0), p2(0,0) { }

void Line::set_p1(Point p1_)
{
  p1 = p1_;
}
void Line::set_p2(Point p2_)
{
  p2 = p2_;
}
Point Line::get_p1()
{
  return p1;
}
Point Line::get_p2()
{
  return p2;
}
