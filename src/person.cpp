#include"reading.h"
#include"mympi.h"
#include"person.h"
MpiSharedArray<Person> *people;
class PersonInitializer {
	public:
		PersonInitializer() {
			int maxPeople=readConfig("playground")->getInt("maxPeople");
			people=new MpiSharedArray<Person>(maxPeople);
		};
		~PersonInitializer() {
			delete people;
		}
	private:
		MpiGlobalInstance mpiGlobalInstance;
};
static PersonInitializer personInitializer;

