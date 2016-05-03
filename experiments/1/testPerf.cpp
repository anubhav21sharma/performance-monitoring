#include <algorithm>
#include <ctime>
#include <iostream>

static unsigned long testFunction(unsigned long n) {
	const unsigned arraySize = 32768;
	int data[arraySize];

	for (unsigned c = 0; c < arraySize; ++c)
		data[c] = std::rand() % 256;

	// !!! With this, the next loop runs faster
//		std::sort(data, data + arraySize);

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

int main(){
	testFunction(1);
	return 0;
}
