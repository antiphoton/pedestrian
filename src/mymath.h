#pragma once
#include<iostream>
struct Vector2;
struct Rect2;

struct Vector2 {
	Vector2(double x=0,double y=0);
	Vector2(const Vector2 &v);
	double x,y;
};
std::ostream & operator << (std::ostream & cout,const Vector2 &v);
struct Rect2 {
	Rect2(double left,double top,double right,double bottom);
	double width() const;
	double height() const;
	Vector2 screenToClient(const Vector2 v) const;
	Vector2 min,max;
};
