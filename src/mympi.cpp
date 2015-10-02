#include<math.h>
#include<string.h>
#include<ctime>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include"mympi.h"
using std::vector;
using std::string;
struct MpiGlobal {
	int size,rank;
};
static MpiGlobal mpiGlobal;
static int globalInstanceCount=0;
MpiGlobalInstance::MpiGlobalInstance() {
	if (globalInstanceCount==0) {
		int argc=0;
		char **argv=new char*[0];
		MPI_Init(&argc,&argv);
		delete[] argv;
		MPI_Comm_rank(MPI_COMM_WORLD,&mpiGlobal.rank);
		MPI_Comm_size(MPI_COMM_WORLD,&mpiGlobal.size);
	}
	globalInstanceCount++;
}
MpiGlobalInstance::~MpiGlobalInstance() {
	globalInstanceCount--;
	if (globalInstanceCount==0) {
		MPI_Finalize();
	}
}
void mpiSync() {
	if (mpiGlobal.size==1) {
		return ;
	}
	if (mpiGlobal.rank==0) {
		int i;
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Status stat;
			int x;
			MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_SYNC,MPI_COMM_WORLD,&stat);
		}
	}
	else {
		int x;
		MPI_Send(&x,1,MPI_INT,0,MPI_SYNC,MPI_COMM_WORLD);
	}
};
void normalizeVector(vector<double> &a) {
	int n=a.size();
	double r=0;
	int i;
	for (i=0;i<n;i++) {
		r+=a[i]*a[i];
	}
	r=sqrt(r);
	for (i=0;i<n;i++) {
		a[i]/=r;
	}
}
double myTimediff(timespec t1,timespec t2) {
	return (t1.tv_sec-t2.tv_sec)+(t1.tv_nsec-t2.tv_nsec)*1e-9;
}
timespec getTime() {
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return now;
}
class TaskManagerReport {
	public:
		TaskManagerReport();
		~TaskManagerReport();
		void push(const string &tag,double supposed,double actual);
	private:
		int n;
		vector<string> vTag;
		vector<double> vSupposed,vActual;
};
TaskManagerReport::TaskManagerReport():n(0) {
}
TaskManagerReport::~TaskManagerReport() {
	if (mpiGlobal.rank!=0) {
		return ;
	}
	FILE *f=fopen("output/cpuStats.txt","w");
	normalizeVector(vSupposed);
	normalizeVector(vActual);
	int i;
	for (i=0;i<n;i++) {
		fprintf(f,"%s\t%f\t%f\n",vTag[i].c_str(),vSupposed[i],vActual[i]);
	}
	fclose(f);
}
void TaskManagerReport::push(const string &tag,double supposed,double actual) {
	if (mpiGlobal.rank!=0) {
		return ;
	}
	n++;
	vTag.push_back(tag);
	vSupposed.push_back(supposed);
	vActual.push_back(actual);
}
MpiTaskManager::MpiTaskManager(const std::string &tag,int count,double supposedCost):count(count),supposedCost(supposedCost),tag(tag) {
	if (mpiGlobal.rank==0) {
		cost1=0;
		cost2=0;
		nextTask=0;
		listen();
	}
}
MpiTaskManager::~MpiTaskManager() {
	if (mpiGlobal.rank==0) {
		write();
	}
}
int MpiTaskManager::apply() {
	if (mpiGlobal.size==1) {
		int ret=nextTask;
		if (ret<count) {
			nextTask++;
			return ret;
		}
		else {
			return -1;
		}
	}
	if (mpiGlobal.rank==0) {
		return -1;
	}
	int x;
	MPI_Status stat;
	MPI_Send(&x,1,MPI_INT,0,MPI_TASK_APPLY,MPI_COMM_WORLD);
	MPI_Recv(&x,1,MPI_INT,0,MPI_TASK_APPLY,MPI_COMM_WORLD,&stat);
	return x;
}
void MpiTaskManager::listen() {
	if (mpiGlobal.size==1) {
		return ;
	}
	int nFinished=0;
	vector<bool> cpuWaiting(mpiGlobal.size,false);
	vector<int> cpuLastTask(mpiGlobal.size,-1);
	vector<timespec> cpuStarttime(mpiGlobal.size);
	int i;
	int nCpuWaiting=0;
	while (nCpuWaiting<mpiGlobal.size-1) {
		MPI_Status stat;
		int ret;
		MPI_Recv(&ret,1,MPI_INT,MPI_ANY_SOURCE,MPI_TASK_APPLY,MPI_COMM_WORLD,&stat);
		timespec now=getTime();
		i=stat.MPI_SOURCE;
		if (cpuWaiting[i]) {
			ret=-1;
		}
		else {
			if (nextTask>=count) {
				ret=-1;
				cpuWaiting[i]=true;
				nCpuWaiting++;
			}
			else {
				ret=nextTask;
				nextTask++;
			}
		}
		if (cpuLastTask[i]!=-1) {
			double delta=myTimediff(now,cpuStarttime[i]);
			cost1+=delta;
			cost2+=delta*delta;
			nFinished++;
		}
		double mu;
		if (nFinished==0) {
			mu=supposedCost;
		}
		else {
			mu=cost1/nFinished;
		}
		double totalStarted=0;
		for (i=1;i<mpiGlobal.size;i++) {
			if (cpuWaiting[i]) {
				continue;
			}
			totalStarted+=myTimediff(now,cpuStarttime[i]);
		}
		double estimated=(mu*(count-nFinished)-totalStarted)/(mpiGlobal.size-1);
		if (estimated<=99) {
			printf("Estimated Remaining: %2d seconds            \r",(int)(estimated+0.5));
		}
		else {
			time_t f=now.tv_sec+now.tv_nsec/1e9+estimated+0.5;
			tm *timeinfo=localtime(&f);
			char timeBuffer[256];
			strftime(timeBuffer,256,"%F %X",timeinfo);
			printf("Estiamted Time:     %s\r",timeBuffer);
		}
		cpuLastTask[stat.MPI_SOURCE]=ret;
		cpuStarttime[stat.MPI_SOURCE]=now;
		MPI_Send(&ret,1,MPI_INT,stat.MPI_SOURCE,MPI_TASK_APPLY,MPI_COMM_WORLD);
	}
}
void MpiTaskManager::write() {

}
MpiSharedMemory::MpiSharedMemory(int size) {
	if (mpiGlobal.size==1) {
		p=malloc(size);
		return ;
	}
	int shmid;
	if (mpiGlobal.rank==0) {
		shmid=shmget(0,size,IPC_CREAT|0666);
		int i;
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Send(&shmid,1,MPI_INT,i,MPI_MEMORY_READY,MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Status stat;
		MPI_Recv(&shmid,1,MPI_INT,0,MPI_MEMORY_READY,MPI_COMM_WORLD,&stat);
	}
	p=(unsigned char*) shmat(shmid,NULL,0);
}
MpiSharedMemory::~MpiSharedMemory() {
	if (mpiGlobal.size==1) {
		free(p);
		return ;
	}
	mpiSync();
	shmdt(p);
}
void *MpiSharedMemory::address() const {
	return p;
}
pthread_mutex_t *SingleThreadLocker::pMutex;
const int SingleThreadLocker::WORKING_RANK=1;
SingleThreadLocker::SingleThreadLocker(bool includingHead):includingHead(includingHead) {
	if (mpiGlobal.size==1) {
		return ;
	}
	if (!includingHead&&mpiGlobal.rank==0) {
		return ;
	}
	if (mpiGlobal.rank==WORKING_RANK) {
		int i;
		for (i=0;i<mpiGlobal.size;i++) {
			if ((i==0&&!includingHead)||i==WORKING_RANK) {
				continue;
			}
			MPI_Status stat;
			int x;
			MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_SINGLETHREAD,MPI_COMM_WORLD,&stat);
		}
	}
	else {
		int x;
		MPI_Send(&x,1,MPI_INT,WORKING_RANK,MPI_SINGLETHREAD,MPI_COMM_WORLD);
	}
}
bool SingleThreadLocker::myDuty() {
	if (mpiGlobal.size==1) {
		return true;
	}
	return mpiGlobal.rank==WORKING_RANK;
}
SingleThreadLocker::~SingleThreadLocker() {
	if (mpiGlobal.size==1) {
		return ;
	}
	if (!includingHead&&mpiGlobal.rank==0) {
		return ;
	}
	if (mpiGlobal.rank==WORKING_RANK) {
		int i;
		for (i=0;i<mpiGlobal.size;i++) {
			if ((i==0&&!includingHead)||i==WORKING_RANK) {
				continue;
			}
			int x;
			MPI_Send(&x,1,MPI_INT,i,MPI_SINGLETHREAD,MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Status stat;
		int x;
		MPI_Recv(&x,1,MPI_INT,WORKING_RANK,MPI_SINGLETHREAD,MPI_COMM_WORLD,&stat);
	}
}
ParallelHistogram::ParallelHistogram(const bool logScale,const bool clamped,const double min,const double max,const int count,const string &filename):logScale(logScale),clamped(clamped),min(logScale?log(min):min),max(logScale?log(max):max),count(count),filename(filename) {
	a=new long[count];
	memset(a,0,sizeof(long)*count);
};
ParallelHistogram::~ParallelHistogram() {
	delete[] a;
}
void ParallelHistogram::tip(double x) {
	if (logScale) {
		x=log(x);
	}
	int i=(x-min)/(max-min)*count;
	if (i<0) {
		if (!clamped) {
			return ;
		}
		else {
			i=0;
		}
	}
	if (i>=count) {
		if (!clamped) {
			return ;
		}
		else {
			i=count-1;
		}
	}
	a[i]++;
}
void ParallelHistogram::merge() {
	if (!mpiGlobal.rank==0) {
		MPI_Send(a,count*8,MPI_CHAR,0,MPI_HIST_MERGE,MPI_COMM_WORLD);
	}
	else {
		MPI_Status mpiStat;
		int i,j;
		long *buffer=new long[count];
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Recv(buffer,count*8,MPI_CHAR,MPI_ANY_SOURCE,MPI_HIST_MERGE,MPI_COMM_WORLD,&mpiStat);
			for (j=0;j<count;j++) {
				a[j]+=buffer[j];
			}
		}
		delete[] buffer;
		long total=0;
		double sum1=0,sum2=0;
		for (i=0;i<count;i++) {
			double x=(i+0.5)/count*(max-min)+min;
			if (logScale) {
				x=exp(x);
			}
			total+=a[i];
			sum1+=x*a[i];
			sum2+=x*x*a[i];
		}
		mu=sum1/total;
		sigma=sqrt(sum2/total-mu*mu);
	}
};
void ParallelHistogram::write() const {
	if (!mpiGlobal.rank==0) {
		return ;
	}
}
