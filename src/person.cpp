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
    tendency.set(0,0);
    stopped=true;
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
            const Vector2 relativeVelocity=that.velocity-velocity;
			if (1) {
				const static double aa2=personConfig->getDouble("exclusionStiffness");
				const static double ab2=personConfig->getDouble("exclusionDecay");
				const static double d2=personConfig->getDouble("exclusionRadius")*2;
                const static double tau=personConfig->getDouble("exclusionTime");
                double b=sqrt(sqr(relativePos.length()+(relativePos+relativeVelocity*tau).length())-relativeVelocity.lengthSqr()*sqr(tau))/2;
                double k;
                if (stopped||velocity.lengthSqr()==0) {
                    k=1;
                }
                else {
                    const static double lambda=personConfig->getDouble("exclusionIsotropy");
                    k=lambda+(1-lambda)*(1+(relativePosDirection&velocity)/velocity.length())/2;
                }
				acceleration+=relativePosDirection*(-aa2*exp((d2-b)/ab2))*k;
			}
			const double goodLeader=that.destGate==destGate?1:-1;//(that.velocity&gravity)/desiredSpeed;
			const static int followEnabled=personConfig->getInt("followEnabled");
			if (followEnabled) {
                const Vector2 thatVelocityDirection=that.velocity*(1/that.velocity.length());
                const double onMyRight=relativePosDirection%thatVelocityDirection;
                const double onMyFront=relativePosDirection&thatVelocityDirection;
				if (goodLeader>0&&onMyFront>0) {
					const static double aa2=personConfig->getDouble("followStiffness");
					const static double ab2=personConfig->getDouble("followDecay");
					const static double d2=personConfig->getDouble("followRadius")*2;
					acceleration+=(relativePosDirection%onMyRight)*(goodLeader*onMyFront*(aa2*exp((d2-relativeDis)/ab2)));
				}
			}
			const static int evasionEnabled=personConfig->getInt("evasionEnabled");
			if (evasionEnabled) {
                const double onMyRight=relativePosDirection%gravity;
                const double onMyFront=relativePosDirection&gravity;
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
		const static double aMaxSqr=sqr(personConfig->getDouble("accelerationMax"));
		if (acceleration.lengthSqr()>aMaxSqr) {
			acceleration.setLengthSqr(aMaxSqr);
		}

	}
}
void Person::move() {
	const static FileParser *personConfig=readConfig("person");
	if (!exist) {
		return ;
	}
	const static double timeStep=frame.simulateStep;
    if (stopped) {
        const static double startingTime=personConfig->getDouble("startingTime");
        const static double startingHalfDegree=personConfig->getDouble("startingHalfDegree")/180*acos(-1.0);
        const static double accelerationMax=personConfig->getDouble("accelerationMax");
        const static double tendencyDecay=exp(-timeStep/startingTime);
        const static double productCutoff=(1-exp(-1.0))*sin(startingHalfDegree)/startingHalfDegree*accelerationMax;
        tendency=tendency*tendencyDecay;
        tendency+=acceleration*timeStep;
        if ((acceleration&tendency)>=productCutoff) {
            stopped=false;
        }
    }
    else {
        const static bool turningObstacle=personConfig->getInt("turningObstacle");
        const Vector2 v0=velocity;
        const Vector2 v2=v0+acceleration*timeStep;
        velocity+=acceleration*timeStep;
        if (turningObstacle&&(v0&v2)<0) {
            velocity.set(0,0);
            tendency.set(0,0);
            stopped=true;
        }
        else {
            velocity=v2;
        }
    }
    if (!stopped) {
        position+=velocity*timeStep;
    }
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

