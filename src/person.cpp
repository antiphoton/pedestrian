#include"reading.h"
#include"mympi.h"
#include"person.h"
MpiSharedArray<Person> *people;
static int maxPeople;
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

