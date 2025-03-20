#pragma once

#include <iostream>
#include <random>

typedef std::mt19937 mt_eng;

// TODO: sort out this whole file, make it less error prone and
// preferably header-only if possible
extern std::random_device ran_dev;
extern mt_eng ran_eng;

// NOTE: Consider using for type = mt_eng::result_type
std::uniform_int_distribution<> iRanGen(int min, int max);

