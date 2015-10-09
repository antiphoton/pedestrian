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
	const static double timeStep=readConfig("frame")->getDouble("step");
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
		}
		if (c==0&&gs.q>0) {
			int personId=getPersonSlot();
			if (personId>=0) {
				people->at(personId).position.set(gs.position);
				people->at(personId).destGate=gs.d;
				const static double mu=readConfig("person")->getDouble("muSpeed");
				const static double sigma=readConfig("person")->getDouble("sigmaSpeed");
				people->at(personId).desiredSpeed=normalDistribution(mu,sigma);
				printf("%f\n",people->at(personId).desiredSpeed);
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

