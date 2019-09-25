#include "hash_functions.h"

#define MAX_S_LEN 4096

hash_t stdHashFunction(hm_size_t iModulo, const char* sKey) {
	size_t len = strnlen(sKey, MAX_S_LEN);
	hash_t h = cityHash(sKey, len);
	
	return h % iModulo;
}

#undef MAX_S_LEN

hash_t stdHashFunction(hm_size_t iModulo, const std::string& sKey) {
	size_t len = sKey.length();
	const char* k = sKey.c_str();
	
	hash_t h = cityHash(k, len);
	
	return h % iModulo;
}

hash_t stdHashFunction(hm_size_t iModulo, long long iKey) {
	return iKey % iModulo;
}
