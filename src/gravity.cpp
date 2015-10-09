#include<math.h>
#include<algorithm>
#include<queue>
#include<string>
#include<vector>
#include"reading.h"
#include"mympi.h"
#include"gravity.h"
using std::make_pair;
using std::pair;
using std::queue;
using std::string;
using std::vector;
vector<double> xSink,ySink;
class GravityInitializer{
	public:
		GravityInitializer() {
			xSink=readConfig("gate")->getDoubleVector("xSink");
			ySink=readConfig("gate")->getDoubleVector("ySink");
		}
		~GravityInitializer() {
		}
};
static GravityInitializer gravityInitializer;

Vector2 getGravity(int iSink,const Vector2 &position) {
	Vector2 a(xSink[iSink]-position.x,ySink[iSink]-position.y);
	double r=sqrt(a.x*a.x+a.y*a.y);
	return Vector2(a.x/r,a.y/r);
}

