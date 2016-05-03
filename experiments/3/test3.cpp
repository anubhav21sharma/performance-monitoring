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

void goodFunction(int SIZE){
	float img[SIZE][SIZE];
	float res[SIZE][SIZE];
	int i,j;	
	for(j=1;j<SIZE-1;j++) {
		for(i=1;i<SIZE-1;i++) {
	        	res[j][i] = 0;
	        	res[j][i] += img[j-1][i-1];
	        	res[j][i] += img[j][i-1];
	        	res[j][i] += img[j+1][i-1];
	        	res[j][i] += img[j-1][i];
	        	res[j][i] += img[j][i];
	        	res[j][i] += img[j+1][i];
       			res[j][i] += img[j-1][i+1];
       			res[j][i] += img[j][i+1];
       	 		res[j][i] += img[j+1][i+1];
       			res[j][i] /= 9;
    		}
	}
}

void badFunction(int SIZE){
	float img[SIZE][SIZE];
	float res[SIZE][SIZE];
	int i,j;	
	for(i=1;i<SIZE-1;i++) {
		for(j=1;j<SIZE-1;j++) {
	        	res[j][i] = 0;
	        	res[j][i] += img[j-1][i-1];
	        	res[j][i] += img[j][i-1];
	        	res[j][i] += img[j+1][i-1];
	        	res[j][i] += img[j-1][i];
	        	res[j][i] += img[j][i];
	        	res[j][i] += img[j+1][i];
       			res[j][i] += img[j-1][i+1];
       			res[j][i] += img[j][i+1];
       	 		res[j][i] += img[j+1][i+1];
       			res[j][i] /= 9;
    		}
	}
}

int main(int argc, char **argv){
	
	int path = 0;
	if (argc > 1) {
		path = argv[1][0];
	}
	
	if(!path){	
		const int LAT_MISSES = 0x412e;

		struct pmuStruct pst = { LAT_MISSES, getpid(), (unsigned long long) -5 };

		int rett = startCountingForThisProcess(pst);
		if (rett < 0) {
			std::cout << "Exiting. Could not start counters!" << std::endl;
			return -1;
		}

		std::cout<<"Good.."<<std::endl;
		goodFunction(8192);	
		
		std::cout<<"Bad.."<<std::endl;
		badFunction(8192);	
		
		unsigned long long cnt = stopCountingAndGiveMeValues(pst);
		std::cout << "Counter Value : " << cnt << std::endl;
	}else{
		std::cout<<"Good.."<<std::endl;
		goodFunction(8192);	
		
		std::cout<<"Bad.."<<std::endl;
		badFunction(8192);	
	}
	return 0;
}
