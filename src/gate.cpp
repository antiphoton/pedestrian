#include"mymath.h"
#include"mytime.h"
#include"gate.h"
#include"reading.h"
#include"person.h"
#include"playground.h"
using std::vector;
Gates::Gates() {
	const FileParser *config=readConfig("gate");
	vector<double> xSource=config->getDoubleVector("xSource");
	vector<double> ySource=config->getDoubleVector("ySource");
	vector<double> rSource=config->getDoubleVector("rSource");
	vector<double> pSource=config->getDoubleVector("pSource");
	vector<double> xSink=config->getDoubleVector("xSink");
	vector<double> ySink=config->getDoubleVector("ySink");
	vector<double> rSink=config->getDoubleVector("rSink");
	nSource=xSource.size();
	nSink=xSink.size();
	aSource=new GateSource[nSource];
	aSink=new GateSink[nSink];
	int i;
	for (i=0;i<nSource;i++) {
		aSource[i].position.x=xSource[i];
		aSource[i].position.y=ySource[i];
		aSource[i].r=rSource[i];
		aSource[i].p=pSource[i];
		aSource[i].q=0;
	}
	for (i=0;i<nSink;i++) {
		aSink[i].x=xSink[i];
		aSink[i].y=ySink[i];
		aSink[i].r=rSink[i];
	}
}
Gates::~Gates() {
	delete[] aSource;
	delete[] aSink;
}
void Gates::update() {
	updateSource();
}
void Gates::updateSource() {
	const vector<CellVector> *pNC;
	int i,j,k;
	for (i=0;i<nSource;i++) {
		GateSource &gs=aSource[i];
		int c=0;
		pNC=playground.getNeighbourCells(gs.position);
		for (j=0;j<(int)pNC->size();j++) {
			int kBegin=playground.indexBegin(pNC->at(j));
			int kEnd=playground.indexEnd(pNC->at(j));
			k=kBegin;
			while (1) {
				k=playground.getNextPerson(k);
				if (k==kEnd) {
					break;
				}
				if (people->at(k).position.disSqr(gs.position)<=sqr(gs.r)) {
					c++;
				}
			}
		}
		if (rand()<gs.p*RAND_MAX) {
			gs.q++;
		}
		if (c==0&&gs.q>0) {
			int personId=playground.getEmptySlot();
			if (personId>=0) {
				people->at(personId).position.set(gs.position);
				playground.addPerson(personId);
				printf("Person %d added by gate %d.\n",personId,i);
				gs.q--;
			}
		}
	}
}
void Gates::updateSink() {
}
Gates gates;

