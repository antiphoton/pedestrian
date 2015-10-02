#include"mymath.h"
#include"gate.h"
#include"reading.h"
using std::vector;
Gates::Gates() {
	const FileParser *config=readConfig("gate");
	vector<double> xSource=config->getDoubleVector("xSource");
	vector<double> ySource=config->getDoubleVector("ySource");
	vector<double> xSink=config->getDoubleVector("xSink");
	vector<double> ySink=config->getDoubleVector("ySink");
	nSource=max(xSource.size(),ySource.size());
	nSink=max(xSink.size(),ySink.size());
	aSource=new GateSource[nSource];
	aSink=new GateSink[nSink];
}
Gates::~Gates() {
	delete[] aSource;
	delete[] aSink;
}
void Gates::update() {
}
Gates gates;

