#include<iostream>
#include<fstream>
#include"mympi.h"
class Trajectory {
	public:
		Trajectory();
		~Trajectory();
		void snapshot();
	private:
		int iFrame;
		FILE *file;
		MpiGlobalInstance mpiGlobalInstance;
};
extern Trajectory trajectory;

