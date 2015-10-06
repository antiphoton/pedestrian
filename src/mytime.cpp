#include"mytime.h"
timespec getTime() {
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return now;
}
double timeDiff(timespec t1,timespec t2) {
	return (t1.tv_sec-t2.tv_sec)+(t1.tv_nsec-t2.tv_nsec)*1e-9;
}

