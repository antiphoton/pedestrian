#include<math.h>
#include"reading.h"
#include"playground.h"
Playground::Playground() {
	const FileParser *config=readConfig("playground");
	width=config->getDouble("width");
	height=config->getDouble("height");
	maxPeople=config->getInt("maxPeople");
	double range=readConfig("person")->getDouble("range");
	nX=ceil(width/range);
	nY=ceil(height/range);
	if (nX<=3) {
		nX=1;
	}
	if (nY<=3) {
		nY=1;
	}
	stepX=width/nX;
	stepY=height/nY;
	linkPrior=new MpiSharedArray<int>(maxPeople+nX*nY*2);
	linkNext=new MpiSharedArray<int>(maxPeople+nX*nY*2);
}
Playground::~Playground() {
	delete linkPrior;
	delete linkNext;
}
int Playground::indexBegin(int y,int x) {
	return maxPeople+y*nX+x;
}
int Playground::indexEnd(int y,int x) {
	return maxPeople+nX*nY+y*nX+x;
}
Playground playground;

