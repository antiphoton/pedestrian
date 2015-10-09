#include"mympi.h"
#include"reading.h"
#include"gate.h"
#include"person.h"
#include"playground.h"
#include"trajectory.h"
int main(int argc,char **argv) {
	for (int iFrame=0;iFrame<100;iFrame++) {
		MpiTaskManager manager("updatePerson",readConfig("playground")->getInt("maxPeople"),1);
		while (1) {
			int personId=manager.apply();
			if (personId==-1) {
				break;
			}
			if (!people->at(personId).exist) {
				continue;
			}
			people->at(personId).think();
			people->at(personId).move();
		}
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
