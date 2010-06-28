#include <cstdlib>
#include <ctime>

#include "rand.hpp"

double util::rand(double _high) {
	static bool initialized = false;
	if(!initialized) {
		srand(time(NULL));
		initialized = true;
	}
	return (std::rand() / (RAND_MAX + 1.0)) * (_high + 1.0);
}

