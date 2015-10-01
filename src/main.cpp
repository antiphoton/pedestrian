#include"mympi.h"
int main(int argc,char **argv) {
	int l=10;
	MpiSharedMemory a(l+1);
	MpiTaskManager m("",l,1);
	char *p=(char *)a.address();
	while (1) {
		int i=m.apply();
		if (i==-1) {
			break ;
		}
		p[i]='A'+i;
	}
	SINGLERUN(
		p[l]=0;
		puts(p);
	)
	return 0;
};
