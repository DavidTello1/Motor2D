#pragma once

#include <random>
#include "PCG/include/pcg_random.hpp"

class RandomNumber
{
public:
	RandomNumber() { // Seed with a real random value, if available
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		rng = rng(seed_source);
	}
	~RandomNumber() {}

	pcg32& GetRNG() { return rng; }

private:
	pcg32 rng;
