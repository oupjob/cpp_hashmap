#include "hash_functions.h"
#include <cmath>
#include <iostream> 

using namespace std;

#define MAX_S_LEN 4096

hash_t cityHashFunction(size_t iModulo, const char* sKey, int ik, int a, int b, int p) 
{
	if (!iModulo)
		iModulo = 7;
	
	
	
	size_t len = strnlen(sKey, MAX_S_LEN);
	hash_t h = cityHash(sKey, len);
	
	return (h + ik) % iModulo;
}

#undef MAX_S_LEN

hash_t cityHashFunction(size_t iModulo, const std::string& sKey, int ik) 
{
	if (!iModulo)
		iModulo = 7;
	
	size_t len = sKey.length();
	const char* k = sKey.c_str();
	
	hash_t h = cityHash(k, len);
	
	return (h + ik) % iModulo;
}

#define MAX_S_LEN 4096

hash_t stdHashFunction(size_t iModulo, const char* sKey, int ik) 
{
	if (!iModulo)
		iModulo = 7;

	size_t 	len = strnlen(sKey, MAX_S_LEN),
			h = 0,
			i_pow = 1;
			
	for(size_t i = 0; i < len; ++i) {
		i_pow *= (i+1);
		h += (sKey[i] + 1) * i_pow;
	}
	
	return (h + ik) % iModulo;
}

#undef MAX_S_LEN

hash_t stdHashFunction(size_t iModulo, const std::string& sKey, int ik) {
	const char* pRawKey = sKey.c_str();
	
	return stdHashFunction(iModulo, pRawKey, ik);
}

// Universal hash function for integers: ((ax + b) mod p) mod m for a stepping to ik 
//  h(x) = (ax + b) mod p, stepping is a (h(x) + ik) mod m 
hash_t stdHashFunction(size_t iModulo, long long iKey, int ik) {
	if (iModulo == 0)
		iModulo = 7;
	
	return (((DEFAULT_A * iKey + DEFAULT_B) % DEFAULT_P) + ik) % iModulo;
}

hash_t stdHashFunction(size_t iModulo, int iKey, int ik) {
	if (iModulo == 0)
		iModulo = 7;
	
	return (((DEFAULT_A * iKey + DEFAULT_B) % DEFAULT_P) + ik) % iModulo;
}

#undef DEFAULT_A
#undef DEFAULT_B
#undef DEFAULT_P
#undef DEFAULT_IK
