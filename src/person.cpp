#include<math.h>
#include"reading.h"
#include"mympi.h"
#include"gravity.h"
#include"person.h"
MpiSharedArray<Person> *people;
int maxPeople;
void Person::think() {
	if (!exist) {
		return ;
	}
	velocity.set(getGravity(destGate,position)*desiredSpeed);
};
void Person::move() {
	if (!exist) {
		return ;
	}
	const static double width=readConfig("playground")->getDouble("width");
	const static double height=readConfig("playground")->getDouble("height");
	const static double timeStep=readConfig("frame")->getDouble("step");
	velocity+=acceleration*timeStep;
	position+=velocity*timeStep;
	position.x-=floor(position.x/width)*width;
	position.y-=floor(position.y/height)*height;
};
class PersonInitializer {
	public:
		PersonInitializer() {
			maxPeople=readConfig("playground")->getInt("maxPeople");
			people=new MpiSharedArray<Person>(maxPeople);
			SINGLERUN {
				int i;
				for (i=0;i<maxPeople;i++) {
					people->at(i).exist=false;
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
		if (people->at(i).exist==false) {
			break;
		}
	}
	if (i==maxPeople) {
		return -1;
	}
	return i;
};
static PersonInitializer personInitializer;

