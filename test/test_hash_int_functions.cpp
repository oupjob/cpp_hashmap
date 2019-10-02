#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include "hash_functions.h"
#include <map>

using namespace std;

#define PRT_FAIL_RESULTS_FOR_INT_FH(h1, h2, function_name, mod, key)\
cout 	<< "FAIL:     " << #function_name << "(M=" << mod << ", Key=" <<  key<<") "\
		<< "haves different iResults for int "\
		<< " (iResult1 = " << h1 << ")"\
		<< ", (iResult2 = " << h2 << ")"\
<< endl;

struct HashDescription 
{	
	int				m_iKey;
	size_t			m_iModulo;
	hash_t			m_iHashValue;
};

bool operator == (const HashDescription& l, const HashDescription& r)
{
	return 
		l.m_iHashValue == 	r.m_iHashValue 	&& 
		l.m_iModulo == 		r.m_iModulo;
}

#define PRT_RESULTS(status, collisions_count, hash_sz, fn) \
cout 	<< status \
		<< #fn << ": " << "\"collisions/calculated hashes\" = " \
		<< collisions_count << "/" << hash_sz \
		<< " (" << ((double)collisions_count/hash_sz) << ")"  \
<< endl;


int main() 
{	
	const int nSampleSize = 1024;
	
	typedef std::vector<HashDescription> HashDescriptionVector;
	HashDescriptionVector 	vHashes;
	
	int iResult = 0;
	bool bResult = true;
	std::string sStatus;
	
	cout << "Please, wait a few second ..." << endl;
	
	// calaculate hashes
	std::random_device oRandom;
	size_t nModules = 64;
	vHashes.reserve(nModules * nSampleSize);
	for (size_t i = 0; i < nSampleSize; ++i) 
	{
		int key = oRandom();
		for(size_t j = 1; j <= nModules; ++j) 
		{
			size_t 	iModulo = 127 * j;
			hash_t  h1 = stdHashFunction(iModulo, key),
					h2 = stdHashFunction(iModulo, key);
			
			bResult = (h1 == h2) && bResult;
			if (!bResult)
				PRT_FAIL_RESULTS_FOR_INT_FH(iModulo, key, stdHashFunction, h1, h2)
			else		
				vHashes.push_back({key, iModulo, h1});
		}
	}
			
	// calculate collisions
	HashDescriptionVector::const_iterator it_i, it_j, it_end = vHashes.end();
	size_t 	iCollisionCount = 0,
			iHashesSize = 0;
	
	for(it_i = vHashes.begin(); it_i != it_end; ++it_i, ++iHashesSize) 
	{
		it_j = it_i;
		// this loop described used do..while, because for loop for this operation view not beauty :)
		do {
			it_j = std::find(std::next(it_j), it_end, *it_i);
			if (it_j != it_end) {
				iCollisionCount++;
			} else {
				break;
			}
		} while(true);
	}
	
	cout << endl;
	
	if (!bResult) {
		sStatus = "NOT A HASH FUNCTION:  ";
	} else if ((double)iCollisionCount/iHashesSize < 0.15 ) {
		sStatus = "A VERY GOOD FUNCTION: ";
	} else if ((double)iCollisionCount/iHashesSize < 0.55 ) {
		sStatus = "A GOOD FUNCTION:      ";
	} else {
		bResult = false;
		sStatus = "A BAD FUNCTION:       ";
	}
	
	PRT_RESULTS(sStatus, iCollisionCount, iHashesSize, stdHashFunction);
	
#define PRT_WIDTH 30
#define SEPARATOR ' '
	
	// print parameters of the test
	cout 	<< "Test parameters:" << endl << left
			<< setw(PRT_WIDTH) << "Random Sample Size: " << setfill(SEPARATOR) << nSampleSize << endl
			<< setw(PRT_WIDTH) << "Modules is " << setfill(SEPARATOR) << "127 * i where i in (1,...," << nModules << "): " << endl
			<< setw(PRT_WIDTH) << "Modules count:" << setfill(SEPARATOR) << nModules << endl;
			
#undef PRT_WIDTH
#undef SEPARATOR
	return !bResult;
}
