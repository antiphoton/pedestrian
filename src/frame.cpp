#include<math.h>
#include"reading.h"
#include"frame.h"
void Frame::init() {
	const FileParser *f=readConfig("frame");
	totalTime=f->getDouble("totalTime");
	simulateStep=f->getDouble("simulateStep");
	printStep=f->getDouble("printStep");
	totalPrint=ceil(totalTime/printStep);
	simulatePerPrint=ceil(printStep/simulateStep);
	totalTime=totalPrint*printStep;
	simulateStep=printStep/simulatePerPrint;
}
Frame frame;
FrameInitializer::FrameInitializer() {
	static bool first=true;
	if (!first) {
		return; 
	}
	first=false;
	frame.init();
}

