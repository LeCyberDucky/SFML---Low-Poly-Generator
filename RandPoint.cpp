#include "stdafx.h" 

#include <cstdint> 
#include <random> 

int_fast32_t randomPoint(int_fast32_t rangeBeg, int_fast32_t rangeEnd)
{
	std::random_device randomSeed;
	std::mt19937 rGenerator(randomSeed());
	std::uniform_int_distribution<size_t> pointRange(rangeBeg, rangeEnd);

	int_fast32_t randPoint = pointRange(rGenerator);

	return randPoint;
}

