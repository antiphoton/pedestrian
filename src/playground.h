#pragma once
#include<vector>
#include"mymath.h"
#include"mympi.h"
struct CellVector {
	int y,x;
	CellVector(int y=0,int x=0);
};
class Playground {
	public:
		Playground();
		~Playground();
		const std::vector<CellVector> *getNeighbourCells(const Vector2 &p) const;
		int indexBegin(const CellVector &cv) const;
		int indexEnd(const CellVector &cv) const;
		int getNextPerson(int x) const;
		void addPerson(int personId);
	private:
		CellVector getCellVector(const Vector2 &p) const;
		MpiSharedArray<int> *linkPrior,*linkNext;
		std::vector<CellVector> **pNeighbourCells;
		double width,height;
		int maxPeople;
		int nX,nY;
		double stepX,stepY;
		MpiGlobalInstance mpiGlobalInstance;
};
extern Playground playground;

