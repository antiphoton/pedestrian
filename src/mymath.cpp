#include<math.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include"mymath.h"
using std::ostream;
using std::string;
double normalDistribution(double mu,double sigma) {
	const double k=1.0/RAND_MAX;
	double x=((0.0+rand()+rand()+rand())*k-1.5);
	x*=2;
	x=mu+x*sigma;
	return x;
}
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
void Vector2::setLengthSqr(double l) {
	double l0=lengthSqr();
	double k=sqrt(l/l0);
	x*=k;
	y*=k;
}
double Vector2::lengthSqr() const {
	return sqr(x)+sqr(y);
}
double Vector2::disSqr(const Vector2 &that) const {
	return sqr(this->x-that.x)+sqr(this->y-that.y);
}
Vector2 Vector2 :: operator - (const Vector2 &that) const {
	return Vector2(this->x-that.x,this->y-that.y);
}
Vector2 & Vector2::operator += (const Vector2 &that) {
	this->x+=that.x;
	this->y+=that.y;
	return *this;
}
Vector2 Vector2::operator *(double k) const {
	return Vector2(x*k,y*k);
}
double Vector2::operator &(const Vector2 &that) const {
	return this->x*that.x+this->y*that.y;
}
inline double sameSide(const Vector2 &vA1,const Vector2 &vA2,const Vector2 &p1,const Vector2 &p2) {
	double yA=vA2.y-vA1.y;
	double xA=vA2.x-vA1.x;
	double l2A=yA*yA+xA*xA;
	double y1=p1.y-vA1.y;
	double x1=p1.x-vA1.x;
	double y2=p2.y-vA1.y;
	double x2=p2.x-vA1.x;
	double s1=xA*y1-yA*x1;
	double s2=xA*y2-yA*x2;
	return s1*s2/l2A;
}
bool intersect(const Vector2 &vA1,const Vector2 &vA2,const Vector2 &vB1,const Vector2 &vB2) {
	const double eps=1e-7;
	double s1=sameSide(vA1,vA2,vB1,vB2);
	double s2=sameSide(vB1,vB2,vA1,vA2);
	return s1<eps&&s2<eps;
}
Vector2 normalDistribution(const Vector2 &mu,const Vector2 &sigma) {
	return Vector2(normalDistribution(mu.x,sigma.x),normalDistribution(mu.y,sigma.y));
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

