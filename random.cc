#include <random>
#include <ctime>

/* NOTE: this is in c++ because rand is not very good */
extern "C" float random_float()
{
	static thread_local std::mt19937 engine {(unsigned long)time(nullptr)};
	std::uniform_real_distribution<float> dist{0.0f, 1.0f};
	return dist(engine);	
}
