#pragma once
#include<iostream>
template<typename T> inline T sqr(T x) {
	return x*x;
}
inline bool randomEvent(double p) {
	return rand()<p*RAND_MAX;
}
double normalDistribution(double mu,double sigma);
struct Vector2;
struct Rect2;
template<typename T> inline T min(const T &x1,const T &x2) {
	return x1<x2?x1:x2;
}
template<typename T> inline T max(const T &x1,const T &x2) {
	return x1>x2?x1:x2;
}

struct Vector2 {
	Vector2(double x=0,double y=0);
	Vector2(const Vector2 &v);
	double x,y;
	void set(double x,double y);
	void set(const Vector2 &v);
	void setLengthSqr(double l);
	double lengthSqr() const;
	double disSqr(const Vector2 &that) const;
	Vector2 operator - (const Vector2 &that) const;
	Vector2 & operator += (const Vector2 &that);
	Vector2 operator * (double k) const;
	double operator & (const Vector2 &that) const;
};
bool intersect(const Vector2 &vA1,const Vector2 &vA2,const Vector2 &vB1,const Vector2 &vB2);
Vector2 normalDistribution(const Vector2 &mu,const Vector2 &sigma);
std::ostream & operator << (std::ostream & cout,const Vector2 &v);
struct Rect2 {
	Rect2(double left=0,double top=0,double right=0,double bottom=0);
	double left,top,right,bottom;
	double width() const;
	double height() const;
};
class Base64Json {
	public:
		Base64Json(FILE *file,int size);
		~Base64Json();
		void push(const void *p);
	private:
		void flush();
		FILE *file;
		const int size;
		unsigned char buffer[3];
		int c;
};

