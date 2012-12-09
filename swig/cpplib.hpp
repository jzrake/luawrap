
#ifndef CPPLIB_H
#define CPPLIB_H

class Point {
public:
  double x, y;
  Point(double x, double y);
} ;

class Line {
public:
  Point p1, p2;
  Line();
  void set_p1(Point p1);
  void set_p2(Point p2);
  Point get_p1();
  Point get_p2();
} ;

#endif // CPPLIB_H
