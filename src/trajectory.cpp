#include"mymath.h"
#include"reading.h"
#include"person.h"
#include"trajectory.h"
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
		fprintf(file,"\"frames\":[\n");
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
	fprintf(file,"{");
	Base64Json *bj;
	int i;
	fprintf(file,"\"exist\":\"");
	bj=new Base64Json(file,sizeof(bool));
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		bj->push(&(p.exist));
	}
	delete bj;
	fprintf(file,"\"");
	fprintf(file,",");
	fprintf(file,"\"position\":\"");
	bj=new Base64Json(file,sizeof(Vector2));
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		bj->push(&(p.position));
	}
	delete bj;
	fprintf(file,"\"");
	fprintf(file,",");
	fprintf(file,"\"velocity\":\"");
	bj=new Base64Json(file,sizeof(Vector2));
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		bj->push(&(p.velocity));
	}
	delete bj;
	fprintf(file,"\"");
	fprintf(file,",");
	fprintf(file,"\"acceleration\":\"");
	bj=new Base64Json(file,sizeof(Vector2));
	for (i=0;i<maxPeople;i++) {
		const Person &p=people->at(i);
		bj->push(&(p.acceleration));
	}
	delete bj;
	fprintf(file,"\"");
	fprintf(file,"}");
	iFrame++;
}

