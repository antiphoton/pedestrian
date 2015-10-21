#include<math.h>
#include<algorithm>
#include<queue>
#include<string>
#include<vector>
#include"reading.h"
#include"mympi.h"
#include"gate.h"
#include"wall.h"
#include"gravity.h"
using std::make_pair;
using std::pair;
using std::queue;
using std::string;
using std::vector;
struct CheckPoint {
	Vector2 p;
	double dis;
	int w;
	CheckPoint(const Vector2 &p,double dis,int w):p(p),dis(dis),w(w) {
	}
};
bool separatedByWalls(const Vector2 &p1,const Vector2 &p2) {
	int k;
	for (k=0;k<(int)walls->size();k++) {
		if (sameSide(walls->at(k).r1,walls->at(k).r2,p1,p2)<0
			&&sameSide(p1,p2,walls->at(k).r1,walls->at(k).r2)<0) {
			return true;
		}
	}
	return false;
}
vector<CheckPoint> createChecklist(const Vector2 &pSink) {
	vector<CheckPoint> ret;
	const double INF=1e100;
	ret.push_back(CheckPoint(pSink,0,-1));
	int i,j,k;
	for (i=0;i<(int)walls->size();i++) {
		ret.push_back(CheckPoint(walls->at(i).r1,INF,i));
		ret.push_back(CheckPoint(walls->at(i).r2,INF,i));
	}
	int n=ret.size();
	vector< vector<double> > f;
	for (i=0;i<n;i++) {
		f.push_back(vector<double>());
		for (j=0;j<n;j++) {
			bool cur=true;
			if (j<i) {
				cur=f[j][i];
			}
			else if (j==i) {
			}
			else {
				Vector2 rij=ret[j].p-ret[i].p;
				cur=cur&&(i==0||(rij&walls->at(ret[i].w).n)>=0);
				cur=cur&&(j==0||(rij&walls->at(ret[j].w).n)<=0);
				cur=cur&&!separatedByWalls(ret[i].p,ret[j].p);
			}
			f.back().push_back(cur?sqrt(ret[i].p.disSqr(ret[j].p)):INF);
		}
	}
	for (k=0;k<n;k++) {
		for (i=0;i<n;i++) {
			for (j=i;j<n;j++) {
				if (f[i][j]>f[i][k]+f[k][j]) {
					f[i][j]=f[i][k]+f[k][j];
					f[j][i]=f[i][j];
				}
			}
		}
	}
	for (i=0;i<n;i++) {
		ret[i].dis=f[0][i];
	}
	return ret;
};
vector< vector<CheckPoint> > *checklist;
GravityInitializer::GravityInitializer() {
	static bool first=true;
	if (!first) {
		return; 
	}
	first=false;
	static GatesInitializer gatesInitializer;
	static WallInitializer wallInitializer;
	vector<double> xSink=readConfig("gate")->getDoubleVector("xSink");
	vector<double> ySink=readConfig("gate")->getDoubleVector("ySink");
	checklist=new vector< vector<CheckPoint> >();
	int n=xSink.size();
	int i;
	for (i=0;i<n;i++) {
		checklist->push_back(createChecklist(Vector2(xSink[i],ySink[i])));
	}
}

Vector2 getGravity(int iSink,const Vector2 &position) {
	static GravityInitializer gravityInitializer;
	const vector<CheckPoint> &c=checklist->at(iSink);
	int n=c.size();
	int best=-1;
	double min=1e100;
	int i;
	for (i=0;i<n;i++) {
		if (separatedByWalls(position,c[i].p)) {
			continue;
		}
		if (best==-1||c[i].dis<min) {
			best=i;
			min=c[i].dis;
		}
	}
	Vector2 a=c[best].p-position;
	double r=sqrt(a.x*a.x+a.y*a.y);
	return Vector2(a.x/r,a.y/r);
}

