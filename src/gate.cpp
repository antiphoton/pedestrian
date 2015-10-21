#include"mymath.h"
#include"mytime.h"
#include"reading.h"
#include"gate.h"
#include"frame.h"
#include"person.h"
#include"playground.h"
using std::vector;
static FrameInitializer frameInitializer;
struct GateSource {
	Vector2 position;
	double r;
	int d;
};
struct GateSink {
	Vector2 position;
	double r;
};
class Gates {
	public:
		Gates();
		~Gates();
		void update();
		void updateSource();
		void updateSink();
		int nSource,nSink;
		GateSource *aSource;
		GateSink *aSink;
};
Gates::Gates() {
	const FileParser *config=readConfig("gate");
	vector<double> xSource=config->getDoubleVector("xSource");
	vector<double> ySource=config->getDoubleVector("ySource");
	vector<double> rSource=config->getDoubleVector("rSource");
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
		aSource[i].d=dSource[i];
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
		const static double maxDensity=readConfig("gate")->getDouble("sourceDensity")/sqr(readConfig("person")->getDouble("exclusionRadius"));
		if (c<maxDensity*sqr(gs.r)) {
			int personId=getPersonSlot();
			if (personId>=0) {
				people->at(personId).reset();
				people->at(personId).id=personId;
				people->at(personId).position.set(uniformDistribution(gs.position,gs.r));
				people->at(personId).destGate=gs.d;
				playground.addPerson(personId);
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
					static bool giveup=readConfig("gate")->getDouble("giveup")!=0;
					if (giveup||people->at(k).destGate==i) {
						playground.deletePerson(k);
					}
				}
			}
		}
	}
}
Gates *gates;
void updateGates() {
	gates->update();
}
GatesInitializer::GatesInitializer() {
	static bool first=true;
	if (!first) {
		return; 
	}
	first=false;
	gates=new Gates();
}
void writeGatesJson(FILE *file) {
	static GatesInitializer gatesInitializer;
	fprintf(file,"\"gates\":{\n");
	fprintf(file,"\"sources\":[\n");
	for (int i=0;i<gates->nSource;i++) {
		const GateSource *g=gates->aSource+i;
		fprintf(file,"[%f,%f,%f]",g->position.x,g->position.y,g->r);
		if (i<gates->nSource-1) {
			fprintf(file,",");
		}
		fprintf(file,"\n");
	}
	fprintf(file,"],\n");
	fprintf(file,"\"sinks\":[\n");
	for (int i=0;i<gates->nSink;i++) {
		const GateSink *g=gates->aSink+i;
		fprintf(file,"[%f,%f,%f]",g->position.x,g->position.y,g->r);
		if (i<gates->nSink-1) {
			fprintf(file,",");
		}
		fprintf(file,"\n");
	}
	fprintf(file,"]\n");
	fprintf(file,"},\n");
}
static GatesInitializer gatesInitializer;

