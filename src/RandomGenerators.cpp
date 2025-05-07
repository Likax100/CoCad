#include "RandomGenerators.h"

std::random_device ran_dev;
mt_eng ran_eng(ran_dev());

std::uniform_int_distribution<> iRanGen(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist;
}
