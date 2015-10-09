#include<math.h>
#include"reading.h"
#include"person.h"
#include"playground.h"
using std::vector;
CellVector::CellVector(int y,int x):y(y),x(x) {
}
bool CellVector::operator == (const CellVector &that) const {
	return this->y==that.y&&this->x==that.x;
}
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
	pNeighbourCells=new vector<CellVector>*[nY*nX];
	for (int y=0;y<nY;y++) {
		for (int x=0;x<nX;x++) {
			vector<CellVector> *p=new vector<CellVector>;
			p->push_back(CellVector(y,x));
			if (nY>3) {
				p->push_back(CellVector((y+1   )%nY,x));
				p->push_back(CellVector((y+nY-1)%nY,x));
			}
			if (nX>3) {
				p->push_back(CellVector(y,(x+1   )%nX));
				p->push_back(CellVector(y,(x+nX-1)%nX));
			}
			pNeighbourCells[y*nX+x]=p;
		}
	}
	stepX=width/nX;
	stepY=height/nY;
	linkPrior=new MpiSharedArray<int>(maxPeople+nX*nY*2);
	linkNext=new MpiSharedArray<int>(maxPeople+nX*nY*2);
	assignedCells=new MpiSharedArray<CellVector>(maxPeople);
	SINGLERUN{
		for (int y=0;y<nY;y++) {
			for (int x=0;x<nX;x++) {
				*(linkNext->address(indexBegin(CellVector(y,x))))=indexEnd(CellVector(y,x));
				*(linkPrior->address(indexEnd(CellVector(y,x))))=indexBegin(CellVector(y,x));
			}
		}
	}
}
Playground::~Playground() {
	delete linkPrior;
	delete linkNext;
}
const vector<CellVector> *Playground::getNeighbourCells(const Vector2 &p) const {
	int y=p.y/stepY;
	int x=p.x/stepX;
	return pNeighbourCells[y*nX+x];
}
int Playground::indexBegin(const CellVector &cv) const {
	return maxPeople+cv.y*nX+cv.x;
}
int Playground::indexEnd(const CellVector &cv) const {
	return maxPeople+nX*nY+cv.y*nX+cv.x;
}
int Playground::getNextPerson(int x) const {
	return *(linkNext->address(x));
}
void Playground::addPerson(int personId) {
	if (people->at(personId).exist==true) {
		return ;
	}
	people->at(personId).exist=true;
	CellVector cv=getCellVector(people->at(personId).position);
	assignedCells->at(personId)=cv;
	addPersonLink(personId,cv);
}
void Playground::updatePerson(int personId) {
	if (people->at(personId).exist==false) {
		return ;
	}
	CellVector cv2=getCellVector(people->at(personId).position);
	if (assignedCells->at(personId)==cv2) {
	}
	else {
		deletePersonLink(personId);
		addPersonLink(personId,cv2);
		assignedCells->at(personId)=cv2;
	}

}
void Playground::deletePerson(int personId) {
	if (people->at(personId).exist==false) {
		return ;
	}
	people->at(personId).exist=false;
	deletePersonLink(personId);
}
void Playground::addPersonLink(int personId,const CellVector &cv) {
	int p3=indexEnd(cv);
	int p1=*(linkPrior->address(p3));
	const int p2=personId;
	*(linkNext->address(p1))=p2;
	*(linkPrior->address(p2))=p1;
	*(linkNext->address(p2))=p3;
	*(linkPrior->address(p3))=p2;
}
void Playground::deletePersonLink(int personId) {
	int p2=personId;
	int p1=linkPrior->at(p2);
	int p3=linkNext->at(p2);
	linkNext->at(p1)=p3;
	linkPrior->at(p3)=p1;
}
CellVector Playground::getCellVector(const Vector2 &p) const {
	return CellVector(p.y/stepY,p.x/stepX);
}
Playground playground;

