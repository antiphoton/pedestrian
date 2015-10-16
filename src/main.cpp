#include<math.h>
#include"mympi.h"
#include"reading.h"
#include"frame.h"
#include"gate.h"
#include"gravity.h"
#include"person.h"
#include"wall.h"
#include"playground.h"
#include"trajectory.h"
static FrameInitializer frameInitializer;
int main(int argc,char **argv) {
	srand(0);//time(0));
	for (int iPrint=0;iPrint<frame.totalPrint;iPrint++) {
		for (int i=0;i<frame.simulatePerPrint;i++) {
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
				updateGates();
			}
		}
		SINGLERUN {
			printf("iPrint=%d\n",iPrint);
			for (int personId=0;personId<maxPeople;personId++) {
				playground.updatePerson(personId);
				Person &p=people->at(personId);
				p.safeToAdd=p.exist==false;
			}
			trajectory.snapshot();
		}
	}
	return 0;
};
