#include"mympi.h"
#include"gate.h"
#include"playground.h"
#include"reading.h"
int main(int argc,char **argv) {
	int i;
	for (i=0;i<100;i++) {
		SINGLERUN {
		printf("i=%d\n",i);
			gates.update();
		}
	}
	return 0;
};
