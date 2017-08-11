#include "randomOperations.h"



RandomOperations::RandomOperations()
{
	mt = std::mt19937(rd());
}


RandomOperations::~RandomOperations()
{
}

int RandomOperations::randomInt(int exclusiveMax)
{
		std::uniform_int_distribution<> dist(0, exclusiveMax - 1);
		return dist(mt);
}

int RandomOperations::randomInt(int min, int max)
{
		std::uniform_int_distribution<> dist(0, max - min);
		return dist(mt) + min;
}

bool RandomOperations::randomBool(double probability)
{
		std::bernoulli_distribution dist(probability);
		return dist(mt);
}
