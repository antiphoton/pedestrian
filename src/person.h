#include"mymath.h"
#include"mympi.h"
struct Person {
	int id;
	Vector2 position,velocity,acceleration;
    Vector2 tendency;
    bool stopped;
	bool exist;
	bool safeToAdd;
	int destGate;
	double desiredSpeed;
	int walkingFrame;
	double effectiveMove;
	void reset();
	void think();
	void move();
};
int getPersonSlot();
extern MpiSharedArray<Person> *people;
extern const int maxPeople;

