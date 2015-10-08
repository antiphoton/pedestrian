#include"mympi.h"
#include"reading.h"
#include"gate.h"
#include"playground.h"
#include"trajectory.h"
int main(int argc,char **argv) {
	int i;
	for (i=0;i<100;i++) {
		SINGLERUN {
			gates.update();
			trajectory.snapshot();
		}
	}
	return 0;
};
