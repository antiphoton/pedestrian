#include<math.h>
#include<string>
#include<vector>
#include"mymath.h"
#include"reading.h"
#include"wall.h"
using std::string;
using std::vector;
static WallInitializer wallInitializer;
WallSegment::WallSegment(const Vector2 &r1,const Vector2 &r2):r1(r1),r2(r2) {
    const static double safeDistance=readConfig("wall")->getDouble("wallSafe");
	d12=r2-r1;
	d12=d12*(1/sqrt(d12.lengthSqr()));
    re1=r1+d12*(-safeDistance);
    re2=r2+d12*(+safeDistance);
	n=d12%(-1);
}
int nWall;
vector<WallSegment> *walls;
const FileParser *config;
WallInitializer::WallInitializer() {
	static bool first=true;
	if (!first) {
		return; 
	}
	first=false;
	config=readConfig("wall");
	vector<string> types=config->getStringVector("type");
	vector<double> x=config->getDoubleVector("x");
	vector<double> y=config->getDoubleVector("y");
	nWall=0;
	walls=new vector<WallSegment>();
	int i;
	for (i=0;i+1<(int)types.size();i++) {
		if (types[i+1]=="l") {
			walls->push_back(WallSegment(Vector2(x[i],y[i]),Vector2(x[i+1],y[i+1])));
			nWall++;
		}
	}
}
void writeWallJson(FILE *file) {
	static WallInitializer wallInitializer;
	fprintf(file,"\"wall\":{\n");
	fprintf(file,"\"segments\":[\n");
	for (int i=0;i<nWall;i++) {
		const WallSegment &w=walls->at(i);
		fprintf(file,"[%f,%f,%f,%f]",w.r1.x,w.r1.y,w.r2.x,w.r2.y);
		if (i<nWall-1) {
			fprintf(file,",");
		}
		fprintf(file,"\n");
	}
	fprintf(file,"]\n");
	fprintf(file,"},\n");
};
Vector2 getWallForce(const Vector2 &position) {
	const static double stiffness=config->getDouble("wallStiffness");
	const static double decay=config->getDouble("wallDecay");
	const static double radius=config->getDouble("wallRadius");
	const static double cutoff=config->getDouble("wallCutoff");
	Vector2 force(0,0);
	int i;
	for (i=0;i<nWall;i++) {
		const WallSegment &w=walls->at(i);
		const Vector2 p1=position-w.r1;
		double dT=w.d12%p1;
		if (dT<=0||dT>cutoff) {
			continue;
		}
		const Vector2 p2=position-w.r2;
		double d1=sqrt(p1.lengthSqr());
		double d2=sqrt(p2.lengthSqr());
		double s1=p1&w.d12;
		double s2=p2&w.d12;
		if (s1>=0&&s2<=0) {
			force+=w.d12%(-stiffness*exp((radius-dT)/decay));
		}
		else {
			if (d1<d2) {
				force+=w.d12%((-stiffness*exp((radius-d1)/decay))*dT/d1);
			}
			else {
				force+=w.d12%((-stiffness*exp((radius-d2)/decay))*dT/d2);
			}
		}
	}
	return force;
}

