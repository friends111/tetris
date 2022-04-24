#include "RandomGenerator.h"

std::random_device RandomGenerator::rd_static;
std::mt19937 RandomGenerator::mt_static = std::mt19937(RandomGenerator::rd_static());

int RandomGenerator::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<> distribution(min, max);

    return distribution(RandomGenerator::mt_static);
}

//Up to 7 decimal places of precision, chance must be 0.0 ~ 100.0
bool RandomGenerator::roll(double percentage)
{
    int max = static_cast<int>(std::round((percentage/100.0) * 10000000.0));
    int roll = RandomGenerator::getRandomInt(1, 10000000);
    
    return roll <= max;
}

bool RandomGenerator::roll()
{
    return RandomGenerator::roll(50.0);
}
