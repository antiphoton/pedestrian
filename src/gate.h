#pragma once
#include<vector>
struct GateSource {
	double x,y,r;
};
struct GateSink {
	double x,y,r;
};
class Gates {
	public:
		Gates();
		~Gates();
		void update();
	private:
		int nSource,nSink;
		GateSource *aSource;
		GateSink *aSink;
};
extern Gates gates;
