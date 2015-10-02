#pragma once
#include<iostream>
struct Vector2;
struct Rect2;
template<typename T> inline T max(const T &x1,const T &x2) {
	return x1>x2?x1:x2;
}

struct Vector2 {
	Vector2(double x=0,double y=0);
	Vector2(const Vector2 &v);
	double x,y;
};
std::ostream & operator << (std::ostream & cout,const Vector2 &v);
struct Rect2 {
	Rect2(double left=0,double top=0,double right=0,double bottom=0);
	double left,top,right,bottom;
	double width() const;
	double height() const;
};

