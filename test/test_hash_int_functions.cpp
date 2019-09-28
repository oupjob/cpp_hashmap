#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include "hash_functions.h"

using namespace std;

#define PRT_FAIL_RESULTS_FOR_INT_FH(h1, h2, function_name, mod, key)\
cout 	<< "FAIL: " << #function_name << "(M=" << mod << ", Key=" <<  key<<") "\
		<< "haves different results for int "\
		<< " (result1 = " << h1 << ")"\
		<< ", (result2 = " << h2 << ")"\
<< endl;

struct HashDescription 
{	
	int				m_iKey;
	int				m_iModulo;
	hash_t			m_iHashValue;
};

bool operator == (const HashDescription& l, const HashDescription& r)
{
	return 
		l.m_iHashValue == 	r.m_iHashValue 	&& 
		l.m_iModulo == 		r.m_iModulo;
}

#define PRT_RESULTS(collisions_count, hash_sz, fn) \
cout 	<< #fn << ": " << "\"collisions/calculated hashes\" = " \
		<< collisions_count << "/" << hash_sz \
		<< " (" << ((double)collisions_count/hash_sz) << ")"  \
<< endl;

int main() 
{	
	const int nSampleSize = 1024;
	int vModules[] = {
		9323, 9337, 9341, 9343, 9349, 9371, 9377, 9391, 9397, 9403, 9413,
		9419, 9421, 9431, 9433, 9437, 9439, 9461, 9463, 9467, 9473, 9479, 9491,
		9497, 9511, 9521, 9533, 9539, 9547, 9551, 9587, 9601, 9613, 9619, 9623,
		9629, 9631, 9643, 9649, 9661, 9677, 9679, 9689, 9697, 9719, 9721, 9733,
		9739, 9743, 9749, 9767, 9769, 9781, 9787, 9791, 9803, 9811, 9817, 9829,
		9833, 9839, 9851, 9857, 9859, 9871, 9883, 9887, 9901, 9907, 9923, 9929,
		9931, 9941, 9949, 9967, 9973
	};
	
			/*7, 11, 13, 17, 19, 23, 29, 31, 37 41, 43, 47, 
		53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 
		109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 
		173, 179, 181, 191, 193, 197, 199 */
	
	typedef std::vector<HashDescription> HashDescriptionVector;
	HashDescriptionVector 	vHashes;
	vHashes.reserve(128 * 128);
	
	int result = 0;
	
	// calaculate hashes
	std::random_device oRandom;
	size_t nModules = sizeof(vModules) / sizeof(int);
	for (size_t i = 0; i < nSampleSize; ++i) 
	{
		int key = oRandom();
		for(size_t j = 0; j < nModules; ++j) 
		{
			hash_t  h1 = stdHashFunction(vModules[j], key),
					h2 = stdHashFunction(vModules[j], key);
					
			if (h1 != h2)
				PRT_FAIL_RESULTS_FOR_INT_FH(vModules[j], key, stdHashFunction, h1, h2)
			else		
				vHashes.push_back({key, vModules[j], h1});
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
				cout 	<< "Collision for `stdHashFunction` (Hash Value = " << it_i->m_iHashValue << ") found between " 
						<<  "(Modulo=" << it_i->m_iModulo << ", Key=" << it_i->m_iKey << ")"
						<< " and (Modulo=" << it_j->m_iModulo << ", Key=" << it_j->m_iKey << ")"
						<< endl;
				iCollisionCount++;
			} else {
				break;
			}
		} while(true);
	}
	
	cout << endl;
	PRT_RESULTS(iCollisionCount, iHashesSize, stdHashFunction);
	
	#define PRT_WIDTH 30
	#define SEPARATOR ' '
	
	// print parameters of the test
	cout 	<< "Test parameters:" << endl << left
			<< setw(PRT_WIDTH) << "Random Sample Size: " << setfill(SEPARATOR) << nSampleSize << endl
			<< setw(PRT_WIDTH) << "Modules is simple numbers: " << setfill(SEPARATOR) 
				<< "from " << vModules[0] << " to " << vModules[nModules - 1] 
			<< endl
			<< setw(PRT_WIDTH) << "Modules count:" << setfill(SEPARATOR) << nModules << endl;
			
	
	return result;
}
