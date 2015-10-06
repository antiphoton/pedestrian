#pragma once
#include<vector>
#include"mymath.h"
struct GateSource {
	Vector2 position;
	double r;
	double p;
	int q;
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
		void updateSource();
		void updateSink();
		int nSource,nSink;
		GateSource *aSource;
		GateSink *aSink;
};
extern Gates gates;
