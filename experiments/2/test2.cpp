
#include <iostream>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


struct pmuStruct {
	int eventToSample;
	int pid;
	int samplingRate;
};

int fd;
int startCountingForThisProcess(struct pmuStruct pst) {
	int ret;
	printf("Starting device test code example : %d\n", getpid());
	fd = open("/dev/pmuIoctlFile", O_RDWR);             // Open the device with read/write access
	if (fd < 0) {
		perror("Failed to open the device!");
		return errno;
	}

	ret = ioctl(fd, 0, &pst);
	if (ret < 0) {
		perror("Failed to start counters!.");
		return ret;
	}
	return 0;
}

unsigned long long stopCountingAndGiveMeValues(struct pmuStruct pst) {
	unsigned long long ret;
	ioctl(fd, 1, &ret);
	return ret;
}

void largeFunction(long long n){
	std::cout<<n<<std::endl;
	long long int a=1;
	for(long long i=0; i<n; i++){
		a=a*i;
	}
}

void mediumFunction(long long n){
	long long int a=1;
	for(long long i=0; i<n; i++){
		a=a*i;
	}
}

void tinyFunction(long long n){
	long long int a=1;
	for(long long i=0; i<n; i++){
		a=a*i;
	}
}

int main(int argc, char **argv){
	
	int path = 0;
	if (argc > 1) {
		path = argv[1][0];
	}
	
	if(path==0){	
		const int CPU_CYCLES = 0x003c;

		struct pmuStruct pst = { CPU_CYCLES, getpid(), (unsigned long long) -1000 };

		int rett = startCountingForThisProcess(pst);
		if (rett < 0) {
			std::cout << "Exiting. Could not start counters!" << std::endl;
			return -1;
		}

		std::cout<<"Tiny.."<<std::endl;
		tinyFunction(1000*1000L);	
		
		std::cout<<"Medium.."<<std::endl;
		mediumFunction(10000L*10000L);	
		
		std::cout<<"Large.."<<std::endl;
		largeFunction(10000LL*100000LL);	

		unsigned long long cnt = stopCountingAndGiveMeValues(pst);
		std::cout << "Counter Value : " << cnt << std::endl;
	}else{
		std::cout<<"Tiny.."<<std::endl;
		tinyFunction(1000*1000L);	
	
		std::cout<<"Medium.."<<std::endl;
		mediumFunction(10000L*10000L);	
	
		std::cout<<"Large.."<<std::endl;
		largeFunction(10000LL*100000LL);	
	}
	return 0;
}
