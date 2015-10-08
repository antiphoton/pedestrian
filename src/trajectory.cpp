#include"reading.h"
#include"person.h"
#include"trajectory.h"
static int maxPeople;
Trajectory trajectory;
Trajectory::Trajectory() {
	SINGLERUN{
		const FileParser *playground=readConfig("playground");
		maxPeople=playground->getInt("maxPeople");
		file=fopen("../monitor/out.trajectory.json","w");
		iFrame=0;
		fprintf(file,"{\n");
		fprintf(file,"\"playground\":{\n");
		fprintf(file,"\"width\":%f,\n",playground->getDouble("width"));
		fprintf(file,"\"height\":%f,\n",playground->getDouble("height"));
		fprintf(file,"\"maxPeople\":%d\n",playground->getInt("maxPeople"));
		fprintf(file,"},\n");
		fprintf(file,"\"position\":[\n");
	}
}
Trajectory::~Trajectory() {
	SINGLERUN {
		fprintf(file,"\n]}\n");
		fclose(file);
	}
}
void Trajectory::snapshot() {
	if (iFrame>0) {
		fprintf(file,",\n");
	}
	fprintf(file,"[");
	int last=0;
	int i,j;
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		if (p.exist) {
			for (j=last;j<i;j++) {
				fprintf(file,",");
			}
			fprintf(file,"[%f,%f]",p.position.x,p.position.y);
			last=i;
		}
	}
	fprintf(file,"]");
	iFrame++;
}

