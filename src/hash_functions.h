#ifndef HAS_FUNCTIONS_H_
#define HAS_FUNCTIONS_H_

#include <string>
#include <string.h>

#include "cityhash/city.h"

typedef size_t hash_t;

typedef unsigned char byte_t;

#if INTPTR_MAX == INT64_MAX // 64-bit
#define cityHash CityHash64
#elif INTPTR_MAX == INT32_MAX // 32-bit
#define cityHash CityHash32
#else 
	#error Unknown pointer size or missing size macros!
#endif

#define DEFAULT_A 7
#define DEFAULT_B 23
#define DEFAULT_P 3571
#define DEFAULT_IK 0

hash_t cityHashFunction(size_t iModulo, const char* sKey, size_t ik = DEFAULT_IK);
hash_t cityHashFunction(size_t iModulo, const std::string& sKey, size_t ik = DEFAULT_IK);

hash_t stdHashFunction(size_t iModulo, const char* sKey, size_t ik = DEFAULT_IK);
hash_t stdHashFunction(size_t iModulo, const std::string& sKey, size_t ik = DEFAULT_IK);
hash_t stdHashFunction(size_t iModulo, long long iKey, size_t ik = DEFAULT_IK);
hash_t stdHashFunction(size_t iModulo, int iKey, size_t ik = DEFAULT_IK);

#endif
