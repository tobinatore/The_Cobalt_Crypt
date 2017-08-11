#pragma once
#include <random>

class RandomOperations
{
public:
	RandomOperations();
	~RandomOperations();

	std::random_device rd;
	std::mt19937 mt;

	//RandomOperations(const RandomOperations&) = default;

	int randomInt(int exclusiveMax);
	
	int randomInt(int min, int max); // inclusive min/max
	
	bool randomBool(double probability = 0.5);
	
};

