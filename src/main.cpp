#include"mympi.h"
#include"reading.h"
#include"gate.h"
#include"gravity.h"
#include"person.h"
#include"playground.h"
#include"trajectory.h"
int main(int argc,char **argv) {
	srand(time(0));
	int totalFrame=readConfig("frame")->getInt("total");
	for (int iFrame=0;iFrame<totalFrame;iFrame++) {
		MpiTaskManager *manager1=new MpiTaskManager("think",readConfig("playground")->getInt("maxPeople"),1);
		while (1) {
			int personId=manager1->apply();
			if (personId==-1) {
				break;
			}
			people->at(personId).think();
		}
		delete manager1;
		SINGLERUN{}
		MpiTaskManager *manager2=new MpiTaskManager("move",readConfig("playground")->getInt("maxPeople"),1);
		while (1) {
			int personId=manager2->apply();
			if (personId==-1) {
				break;
			}
			people->at(personId).move();
		}
		delete manager2;
		SINGLERUN {
			for (int personId=0;personId<maxPeople;personId++) {
				playground.updatePerson(personId);
			}
			gates.update();
			trajectory.snapshot();
		}
	}
	return 0;
};
