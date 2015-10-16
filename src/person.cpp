#include<math.h>
#include<vector>
#include"reading.h"
#include"mympi.h"
#include"frame.h"
#include"gravity.h"
#include"wall.h"
#include"person.h"
#include"playground.h"
using std::vector;
static FrameInitializer frameInitializer;
MpiSharedArray<Person> *people;
const int maxPeople=readConfig("playground")->getInt("maxPeople");
void Person::reset() {
	velocity.set(0,0);
	const static double mu=readConfig("person")->getDouble("muSpeed");
	const static double sigma=readConfig("person")->getDouble("sigmaSpeed");
	desiredSpeed=normalDistribution(mu,sigma);
	effectiveMove=0;
	walkingFrame=0;
}
void Person::think() {
	const static FileParser *personConfig=readConfig("person");
	if (!exist) {
		return ;
	}
	Vector2 gravity=getGravity(destGate,position);
	double impatience=0;
	if (walkingFrame>0) {
		impatience=1-effectiveMove/walkingFrame/desiredSpeed;
	}
	effectiveMove+=gravity&velocity;
	walkingFrame++;
	acceleration.set(0,0);
	acceleration+=getWallForce(position);
	if (1) {
		const static double tau=personConfig->getDouble("relaxationTime");
		const static double pF=personConfig->getDouble("patienceFactor");
		const static double ipF=personConfig->getDouble("impatienceFactor");
		double vat=desiredSpeed*(pF*(1-impatience)+ipF*impatience);
		acceleration+=(gravity*vat-velocity)*(1/tau);
	}
	const vector<CellVector> *pNC=playground.getNeighbourCells(position);
	for (int i=0;i<(int)pNC->size();i++) {
		int jBegin=playground.indexBegin(pNC->at(i));
		int jEnd=playground.indexEnd(pNC->at(i));
		int j=jBegin;
		while (1) {
			j=playground.getNextPerson(j);
			if (j==jEnd) {
				break;
			}
			const Person &that=people->at(j);
			if (id==that.id) {
				continue;
			}
			const Vector2 relativePos=that.position-position;
			const double relativeDis=sqrt(relativePos.lengthSqr());
			const Vector2 relativePosDirection=relativePos*(1/relativeDis);
			if (1) {
				const static double aa2=personConfig->getDouble("exclusionStiffness");
				const static double ab2=personConfig->getDouble("exclusionDecay");
				const static double d2=personConfig->getDouble("exclusionRadius")*2;
				acceleration+=relativePosDirection*(-aa2*exp((d2-relativeDis)/ab2));
			}
			const double goodLeader=(that.velocity&gravity)/desiredSpeed;
			const double onMyRight=relativePosDirection%gravity;
			const double onMyFront=relativePosDirection&gravity;
			const static int followEnabled=personConfig->getInt("followEnabled");
			if (followEnabled) {
				if (goodLeader>0&&onMyFront>0) {
					const static double aa2=personConfig->getDouble("followStiffness");
					const static double ab2=personConfig->getDouble("followDecay");
					const static double d2=personConfig->getDouble("followRadius")*2;
					acceleration+=(relativePosDirection%onMyRight)*(goodLeader*onMyFront*(aa2*exp((d2-relativeDis)/ab2)));
				}
			}
			const static int evasionEnabled=personConfig->getInt("evasionEnabled");
			if (evasionEnabled) {
				if (goodLeader<0&&onMyFront>0) {
					const static double aa2=personConfig->getDouble("evasionStiffness");
					const static double ab2=personConfig->getDouble("evasionDecay");
					const static double d2=personConfig->getDouble("evasionRadius")*2;
					acceleration+=(relativePosDirection%onMyRight)*(goodLeader*onMyFront*(aa2*exp((d2-relativeDis)/ab2)));
				}
			}
		}
	}
	if (1) {
		const static double aMaxSqr=sqr(personConfig->getDouble("accelerationMax")*(personConfig->getDouble("muSpeed")+personConfig->getDouble("sigmaSpeed")*3)/personConfig->getDouble("relaxationTime")*personConfig->getDouble("impatienceFactor"));
		if (acceleration.lengthSqr()>aMaxSqr) {
			acceleration.setLengthSqr(aMaxSqr);
		}

	}
}
void Person::move() {
	if (!exist) {
		return ;
	}
	const static double width=readConfig("playground")->getDouble("width");
	const static double height=readConfig("playground")->getDouble("height");
	const static double timeStep=frame.simulateStep;
	position+=velocity*timeStep;
	position.x-=floor(position.x/width)*width;
	position.y-=floor(position.y/height)*height;
	velocity+=acceleration*timeStep;
};
class PersonInitializer {
	public:
		PersonInitializer() {
			people=new MpiSharedArray<Person>(maxPeople);
			SINGLERUN {
				int i;
				for (i=0;i<maxPeople;i++) {
					people->at(i).exist=false;
					people->at(i).safeToAdd=true;
				}
			}
		};
		~PersonInitializer() {
			delete people;
		}
	private:
		MpiGlobalInstance mpiGlobalInstance;
};
int getPersonSlot() {
	int i;
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		if (p.exist==false&&p.safeToAdd==true) {
			break;
		}
	}
	if (i==maxPeople) {
		return -1;
	}
	return i;
};
static PersonInitializer personInitializer;

