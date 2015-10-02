#pragma once
#include"mymath.h"
#include"mympi.h"
class Playground {
	public:
		Playground();
		~Playground();
	private:
		double width,height;
		int maxPeople;
		int nX,nY;
		double stepX,stepY;
		MpiSharedArray<int> *linkPrior,*linkNext;
		int indexBegin(int y,int x);
		int indexEnd(int y,int x);
		MpiGlobalInstance mpiGlobalInstance;
};
extern Playground playground;

