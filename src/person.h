#include"mymath.h"
#include"mympi.h"
struct Person {
	Vector2 position,velocity,acceleration;
	bool exist;
	void think();
	void move();
};
int getPersonSlot();
extern MpiSharedArray<Person> *people;
extern int maxPeople;

