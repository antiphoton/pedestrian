#include"reading.h"
#include"playground.h"
Playground::Playground() {
	const FileParser *p=readConfig("playground");
	printf("width=%f\n",p->getDouble("width"));
}


