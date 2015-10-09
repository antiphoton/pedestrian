#include<math.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include"mymath.h"
using std::ostream;
using std::string;
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
Vector2 & Vector2::operator += (const Vector2 &that) {
	this->x+=that.x;
	this->y+=that.y;
	return *this;
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
Base64Json::Base64Json(FILE *file,int size):file(file),size(size),c(0) {
}
Base64Json::~Base64Json() {
	flush();
}
void Base64Json::push(const void *p) {
	int i;
	for (i=0;i<size;i++) {
		if (c==3) {
			flush();
		}
		buffer[c]=((unsigned char *)p)[i];
		c++;
	}
}
void Base64Json::flush() {
	const static string lib("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	if (c==0) {
		return ;
	}
	if (c!=3) {
		int i;
		for (i=c;i<3;i++) {
			buffer[i]=0;
		}
	}
	char y0=lib[                      ((buffer[0]&0xFC)>>2)];
	char y1=lib[((buffer[0]&0x03)<<4)|((buffer[1]&0xF0)>>4)];
	char y2=lib[((buffer[1]&0x0F)<<2)|((buffer[2]&0xC0)>>6)];
	char y3=lib[((buffer[2]&0x3F)   )                      ];
	if (c<=2) {
		y3='=';
	}
	if (c<=1) {
		y2='=';
	}
	fprintf(file,"%c%c%c%c",y0,y1,y2,y3);
	c=0;
}

