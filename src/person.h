#include"mymath.h"
#include"mympi.h"
struct Person {
	Vector2 position,velocity;
	bool exist;
};
int getPersonSlot();
extern MpiSharedArray<Person> *people;

