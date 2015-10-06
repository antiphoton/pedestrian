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
void Vector2::set(double x,double y) {
	this->x=x;
	this->y=y;
}
void Vector2::set(const Vector2 &v) {
	this->x=v.x;
	this->y=v.y;
}
double Vector2::disSqr(const Vector2 &that) const {
	return sqr(this->x-that.x)+sqr(this->y-that.y);
}
ostream & operator << (ostream & cout,const Vector2 &v) {
	return cout<<"("<<v.x<<","<<v.y<<")";
}
Rect2::Rect2(double left,double top,double right,double bottom):left(left),top(top),right(right),bottom(bottom) {
}
double Rect2::width() const {
	return right-left;
}
double Rect2::height() const {
	return bottom-top;
}

