#include"mymath.h"
#include"mytime.h"
#include"reading.h"
#include"gate.h"
#include"frame.h"
#include"person.h"
#include"playground.h"
using std::vector;
static FrameInitializer frameInitializer;
Gates::Gates() {
	const FileParser *config=readConfig("gate");
	vector<double> xSource=config->getDoubleVector("xSource");
	vector<double> ySource=config->getDoubleVector("ySource");
	vector<double> rSource=config->getDoubleVector("rSource");
	vector<double> pSource=config->getDoubleVector("pSource");
	vector<double> dSource=config->getDoubleVector("dSource");
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
		aSource[i].d=dSource[i];
		aSource[i].q=0;
	}
	for (i=0;i<nSink;i++) {
		aSink[i].position.x=xSink[i];
		aSink[i].position.y=ySink[i];
		aSink[i].r=rSink[i];
	}
}
Gates::~Gates() {
	delete[] aSource;
	delete[] aSink;
}
void Gates::update() {
	updateSource();
	updateSink();
}
void Gates::updateSource() {
	const static double timeStep=frame.simulateStep;
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
		if (randomEvent(gs.p*timeStep)) {
			gs.q++;
			gs.q=1;
		}
		const static double maxDensity=readConfig("gate")->getDouble("sourceDensity")/sqr(readConfig("person")->getDouble("exclusionRadius"));
		if (c<maxDensity*sqr(gs.r)&&gs.q>0) {
			int personId=getPersonSlot();
			if (personId>=0) {
				people->at(personId).reset();
				people->at(personId).id=personId;
				people->at(personId).position.set(normalDistribution(gs.position,Vector2(gs.r/3,gs.r/3)));
				people->at(personId).destGate=gs.d;
				playground.addPerson(personId);
				gs.q--;
			}
		}
	}
}
void Gates::updateSink() {
	const vector<CellVector> *pNC;
	int i,j,k;
	for (i=0;i<nSink;i++) {
		GateSink &gs=aSink[i];
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
					playground.deletePerson(k);
				}
			}
		}
	}
}
Gates gates;

