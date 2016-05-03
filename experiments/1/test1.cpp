#include <iostream>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <perfmon/pfmlib_perf_event.h>


#define N 30

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

static unsigned long testFunction() {
	const unsigned arraySize = 32768;
	int data[arraySize];

	for (unsigned c = 0; c < arraySize; ++c)
		data[c] = std::rand() % 256;
	unsigned long sum = 0;
	for (unsigned i = 0; i < 100000; ++i) {
		// Primary loop
		for (unsigned c = 0; c < arraySize; ++c) {
			if (data[c] >= 128)
				sum += data[c];
		}
	}
	return sum;
}

int main(int argc, char **argv) {
	int path = 0;
	if (argc > 1) {
		path = argv[1][0];
	}

	if (path) {
		testFunction();
	}
	else {

		const int CPU_CYCLES = 0x003c;
		const int BRANCH_MISPREDICTIONS = 0x00C5;

		struct pmuStruct pst = { BRANCH_MISPREDICTIONS, getpid(), (unsigned long long) -100 };

		int rett = startCountingForThisProcess(pst);
		if (rett < 0) {
			std::cout << "Exiting. Could not start counters!" << std::endl;
			return -1;
		}

		testFunction();

		unsigned long long cnt = stopCountingAndGiveMeValues(pst);
		std::cout << "Counter Value : " << cnt << std::endl;
	}
	return 0;
}
