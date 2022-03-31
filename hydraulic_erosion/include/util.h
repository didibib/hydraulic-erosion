#pragma once

//#include "perlin_noise.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace util
{
	typedef unsigned int	uint;
	typedef unsigned char	byte;
	
	namespace random
	{
		// Deterministic rng
		static uint seed = 0x06942069;
		inline uint Uint()
		{
			seed ^= seed << 13;
			seed ^= seed >> 17;
			seed ^= seed << 5;
			return seed;
		}
		inline float Float() { return Uint() * 2.3283064365387e-10f; }
		inline float Range(float range) { return Float() * range; }

		//static const siv::PerlinNoise perlin{ seed };
	}

	namespace math
	{
		inline float Map(float value, float in_start, float in_stop, float out_start, float out_stop)
		{
			return out_start + (out_stop - out_start) * ((value - in_start) / (in_stop - in_start));
		}
	};

	// https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	inline std::string getFileContents(const char* filename)
	{
		std::ifstream in(filename, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}
}