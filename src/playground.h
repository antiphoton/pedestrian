#pragma once
#include<vector>
#include"mymath.h"
#include"mympi.h"
struct CellVector {
	int y,x;
	CellVector(int y=0,int x=0);
	bool operator == (const CellVector &that) const;
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
		void updatePerson(int personId);
		void deletePerson(int personId);
		MpiSharedArray<CellVector> *assignedCells;
	private:
		CellVector getCellVector(const Vector2 &p) const;
		void addPersonLink(int personId,const CellVector &cv);
		void deletePersonLink(int personId);
		MpiSharedArray<int> *linkPrior,*linkNext;
		std::vector<CellVector> **pNeighbourCells;
		double width,height;
		int maxPeople;
		int nX,nY;
		double stepX,stepY;
		MpiGlobalInstance mpiGlobalInstance;
};
extern Playground playground;

