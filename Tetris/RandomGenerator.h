#pragma once
#include <random>
#include <iostream>

class RandomGenerator
{
private:
	static std::random_device rd_static;
	static std::mt19937 mt_static;
public:
	static int getRandomInt(int min, int max);
	static bool roll(double chance);
	static bool roll();
};
