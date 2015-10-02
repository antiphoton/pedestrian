#include"person.h"
Person *people;
class PersonInitializer {
	public:
		PersonInitializer() {
			MpiSharedMemory aaa(100);
		};
		~PersonInitializer() {
		}
	private:
		MpiGlobalInstance mpiGlobalInstance;
};
static PersonInitializer personInitializer;

