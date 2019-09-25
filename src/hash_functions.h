#ifndef HAS_FUNCTIONS_H_
#define HAS_FUNCTIONS_H_

#include <string>
#include <string.h>

#include "cityhash/city.h"



typedef size_t hm_size_t;
typedef unsigned long long hash_t;

typedef unsigned char byte_t;

#if INTPTR_MAX == INT64_MAX // 64-bit
#define cityHash CityHash64
#elif INTPTR_MAX == INT32_MAX // 32-bit
#define cityHash CityHash32
#else 
	#error Unknown pointer size or missing size macros!
#endif

hash_t stdHashFunction(hm_size_t iModulo, const char* sKey);
hash_t stdHashFunction(hm_size_t iModulo, const std::string& sKey);
hash_t stdHashFunction(hm_size_t iModulo, long long iKey);

// #define IS_HASH_T_ALIGNED(v) !(v % sizeof(hash_t))
// 
// template <hm_size_t iModulo, typename KeyT> 
// hash_t stdHashFunction(const KeyT& key)
// {
// 	hm_size_t iKeyTSize = sizeof(key);
// 	hash_t iResult = 0;
// 	
// 	if (!IS_HASH_T_ALIGNED(iKeyTSize)) {
// 		const byte_t 	*p = reinterpret_cast<const byte_t*>(&key), 
// 						*end = p + iKeyTSize - 1;
// 						
// 		for(; p != end; ++p)
// 			iResult ^= *p;
// 		
// 	} else {
// 		const hash_t 	*p = reinterpret_cast<const hash_t*>(&key), 
// 						*end = p + iKeyTSize - 1;
// 		for(; p != end; ++p)
// 			iResult ^= *p;
// 	}
// 	
// 	return iResult % iModulo;
// }
// 
// #undef IS_HASH_T_ALIGNED

// template <hm_size_t iModulo, typename KeyT> 
// struct StdHashFunction 
// {
// 	hash_t operator () (const KeyT& key)
// 	{
// 		hm_size_t iKeyTSize = sizeof(key);
// 		hash_t iResult = 0;
// 		
// 		if (!IS_HASH_T_ALIGNED(iKeyTSize)) {
// 			const byte_t 	*p = reinterpret_cast<const byte_t*>(&key), 
// 							*end = p + iKeyTSize - 1;
// 							
// 			for(; p != end; ++p)
// 				iResult ^= *p;
// 			
// 		} else {
// 			const hash_t 	*p = reinterpret_cast<const hash_t*>(&key), 
// 							*end = p + iKeyTSize - 1;
// 			for(; p != end; ++p)
// 				iResult ^= *p;
// 		}
// 		
// 		return iResult % iModulo;
// 	}
// };
// 

// #define MAX_S_LEN 4098
// 
// #include <iostream>
// using namespace std;
// 
// hash_t stdHashFunction(hm_size_t iModulo, const char* key) 
// {	
// 	const size_t iStrnLen = strnlen(key, MAX_S_LEN);
// 	const byte_t 	*p = reinterpret_cast<const byte_t*>(key), 
// 					*end = p + iStrnLen - 1;
// 	hash_t iResult = 0;
// 	
// 	for(; p != end; ++p)
// 		iResult ^= *p;
// 	
// 	return iResult % iModulo;
// };
// 
// #undef MAX_S_LEN
// 
// hash_t stdHashFunction(hm_size_t iModulo, const std::string& key)
// {
// 	std::string::const_iterator it_p = key.begin(), 
// 								it_end = key.end();
// 	
// 	hash_t iResult = 0;
// 	for(; it_p != it_end; ++it_p)
// 		iResult ^= *it_p;
// 	
// 	return iResult % iModulo;
// }
// 
// hash_t stdHashFunction(hm_size_t iModulo, const int& key) 
// {
// 	return key % iModulo;
// }

// #define MAX_S_LEN 4098
// 
// template <hm_size_t iModulo> 
// struct StdHashFunction<iModulo, char*> 
// {
// 	hash_t operator () (const char* key) 
// 	{
// 		const size_t iStrnLen = strnlen(key, MAX_S_LEN);
// 		const byte_t 	*p = reinterpret_cast<const byte_t*>(key), 
// 						*end = p + iStrnLen - 1;
// 		hash_t iResult = 0;
// 		for(; p != end; ++p)
// 			iResult ^= *p;
// 		
// 		return iResult % iModulo;
// 	}
// };
// 
// #undef MAX_S_LEN
// 
// template <hm_size_t iModulo>  
// struct StdHashFunction<iModulo, std::string&> 
// {
// 	hash_t operator () (const std::string& key)
// 	{
// 		std::string::iterator 	it_p = key.begin(), 
// 								it_end = key.end();
// 		
// 		hash_t iResult = 0;
// 		for(; it_p != it_end; ++it_p)
// 			iResult ^= *it_p;
// 		
// 		return iResult % iModulo;
// 	}
// };
// 
// template <hm_size_t iModulo> 
// struct StdHashFunction<iModulo, int>
// {
// 	hash_t operator () (const int& key) 
// 	{
// 		return key % iModulo;
// 	}
// };

#endif
