#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

const int maxTicks = 100;
const int populationSize = 50;

class Patch {
public:
	int chemical;
	int food;
	bool nest;
	int nestScent;
	int foodSourceNumber;

	Patch(): chemical(0), food(0), nest(false), nestScent(0), foodSourceNumber(0){}
};