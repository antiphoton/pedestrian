#include<math.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include"mymath.h"
using std::ostream;

Vector2::Vector2(double x,double y):x(x),y(y) {
}
Vector2::Vector2(const Vector2 &v) {
	x=v.x;
	y=v.y;
}
ostream & operator << (ostream & cout,const Vector2 &v) {
	return cout<<"("<<v.x<<","<<v.y<<")";
}
Rect2::Rect2(double left,double top,double right,double bottom):min(left,top),max(right,bottom) {
}
double Rect2::width() const {
	return max.x-min.x;
}
double Rect2::height() const {
	return max.y-min.y;
}
Vector2 Rect2::screenToClient(const Vector2 v) const {
	Vector2 ret(
		(v.x-min.x)/width(),
		(v.y-min.y)/height()
	);
	return ret;
}
