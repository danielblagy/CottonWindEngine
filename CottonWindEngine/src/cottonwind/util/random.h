#pragma once

#include <cstdlib>
#include <ctime>


namespace cotwin
{
	// TODO : use c++ random
	namespace random
	{
		inline void init()
		{
			std::srand(std::time(0));
		}

		// returns a random bool (either true or false)
		inline bool b(int lowest, int highest)
		{
			return std::rand() % 2 == 0;
		}
		
		// returns a random integer in range [lowest, highest]
		inline int i(int lowest, int highest)
		{
			return std::rand() % highest + lowest;
		}

		// returns a random integer in range (lowest, highest) (exluding)
		inline int ie(int lowest, int highest)
		{
			return std::rand() % highest + lowest;
		}

		// returns a random float in range (0, 1) (excluding 0 and 1)
		inline float f()
		{
			return (float) std::rand() / RAND_MAX;
		}
	}
}