struct Frame {
	double totalTime;
	double simulateStep;
	double printStep;
	int totalPrint;
	int simulatePerPrint;
	void init();
};
extern Frame frame;
struct FrameInitializer {
	FrameInitializer();
};
