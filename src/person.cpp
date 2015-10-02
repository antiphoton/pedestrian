#include"person.h"
Person *people;
class PersonInitializer {
	public:
		PersonInitializer() {
		};
		~PersonInitializer() {
		}
	private:
		MpiGlobalInstance mpiGlobalInstance;
};
static PersonInitializer personInitializer;

