#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "hash_functions.h"

using namespace std;

#define PRT_FAIL_RESULTS_FOR_STR_FH(h1, h2, function_name)\
cout 	<< "FAIL: " << #function_name << "(" << vModules[i] << ", " << vStrs[j] <<") "\
		<< "haves different results "\
		<< "\n\tfor std::string (result = " << h1 << ")"\
		<< "\n\tfor C string (result = " << h2 << ")"\
<< endl;

struct HashDescription 
{
	enum FuncType {
		kCityHash,
		kStdHash
	} 				m_kFuncType;
	
	size_t			m_iVKeysIndex;
	int				m_iModulo;
	hash_t			m_iHashValue;
};

bool operator == (const HashDescription& l, const HashDescription& r)
{
	return 
		l.m_iHashValue == 	r.m_iHashValue 	&& 
		l.m_iModulo == 		r.m_iModulo 	&&
		l.m_kFuncType == 	r.m_kFuncType;
}

#define PRT_RESULTS(collisions_count, hash_sz, fn) \
cout 	<< #fn << ": " << "\"collisions/calculated hashes\" = " \
		<< collisions_count << "/" << hash_sz \
		<< " (" << ((double)collisions_count/hash_sz) << ")"  \
<< endl;

int main() 
{	
	std::vector<std::string> vStrs;
	vStrs.reserve(128 * 128);
	vStrs.push_back("abc");
	vStrs.push_back("aac");
	vStrs.push_back("aab");
	vStrs.push_back("aaa");
	vStrs.push_back("abb");
	vStrs.push_back("acc");
	vStrs.push_back("acb");
	vStrs.push_back("bac");
	vStrs.push_back("cbc");
	vStrs.push_back("ADASD:L:alkosiwot01-12??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-13??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-14??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-15??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-16??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-17??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-18??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-19??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-22??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-32??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-42??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-52??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-62??  adasd");
	vStrs.push_back("ADASD:L:alkosiwot01-72??  adasd");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1312)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1301)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1302)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1303)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1304)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1305)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1306)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1360)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1361)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1201)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1202)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1203)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1204)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1205)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1250)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1200)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	vStrs.push_back("A long string, ASDASARJEJKJALK LJFLKSADADSQWEWADSA!1255)_ !#@$##%asdsad234e5ye4trhfy7t8tyiykio0u908!!@#$@#");
	
	int vModules[] = {
		7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 
		53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 
		109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 
		173, 179, 181, 191, 193, 197, 199 
	};
	
	typedef std::vector<HashDescription> HashDescriptionVector;
	HashDescriptionVector 	vHashes;
	vHashes.reserve(128 * 128);
	
	int result = 0;
		
	std::string vFnNames[] = {
		"cityHash",
		"stdHash"
	};
	
	// calaculate hashes
	size_t nStrs = vStrs.size();
	size_t nModules = sizeof(vModules) / sizeof(int);
	for (size_t i = 0; i < nModules; ++i) 
	{
		for(size_t j = 0; j < nStrs; ++j) 
		{
			hash_t h1 = cityHashFunction(vModules[i], vStrs[j]);
			hash_t h2 = cityHashFunction(vModules[i], vStrs[j].c_str());
			
			if (h1 != h2) {
				PRT_FAIL_RESULTS_FOR_STR_FH(h1, h2, cityHashFunction);
				result = 1;
			} else {
				vHashes.push_back({HashDescription::kCityHash, j, vModules[i], h1});
			}
			
			hash_t h3 = stdHashFunction(vModules[i], vStrs[j]);
			hash_t h4 = stdHashFunction(vModules[i], vStrs[j].c_str());
			
			if (h3 != h4) {
				PRT_FAIL_RESULTS_FOR_STR_FH(h3, h4, stdHashFunction);
				result = 1;
			} else {
				vHashes.push_back({HashDescription::kStdHash, j, vModules[i], h3});
			}
		}
	}
			
	// calculate collisions
	HashDescriptionVector::const_iterator it_i, it_j, it_end = vHashes.end();
	size_t 	iCityHashCollisionCount = 0,
			iStdHashCollisionCount = 0,
			iHashesSize = 0;
	
	for(it_i = vHashes.begin(); it_i != it_end; ++it_i, ++iHashesSize) 
	{
		it_j = it_i;
		// this loop described used do..while, because for loop for this operation view not beauty :)
		do {
			it_j = std::find(std::next(it_j), it_end, *it_i);
			if (it_j != it_end) {
				cout << "Collision (Hash Value = " << it_i->m_iHashValue << ") found between <" 
				<< vFnNames[it_i->m_kFuncType] << "(Modulo=" << it_i->m_iModulo << ", Key=" << vStrs[it_i->m_iVKeysIndex] << ")"
				<< "> and <" 
				<< vFnNames[it_j->m_kFuncType] << "(Modulo=" << it_j->m_iModulo << ", Key=" << vStrs[it_j->m_iVKeysIndex] << ")"
				<< ">"
				<< endl;
				
				if (it_i->m_kFuncType == HashDescription::kCityHash)
					iCityHashCollisionCount++;
				else if (it_i->m_kFuncType == HashDescription::kStdHash)
					iStdHashCollisionCount++;
				
			} else {
				break;
			}
		} while(true);
	}
	
	cout << endl;
	PRT_RESULTS(iCityHashCollisionCount, iHashesSize, cityHash);
	PRT_RESULTS(iStdHashCollisionCount, iHashesSize, stdHash);
	PRT_RESULTS((iStdHashCollisionCount + iCityHashCollisionCount), iHashesSize, "Summary");
	
	#define PRT_WIDTH 30
	#define SEPARATOR ' '
		// print parameters of the test
	cout 	<< "Test parameters:" << endl << left
			<< setw(PRT_WIDTH) << "Sample Size: " << setfill(SEPARATOR) << nStrs << endl
			<< setw(PRT_WIDTH) << "Modules is simple numbers: " << setfill(SEPARATOR) 
				<< "from " << vModules[0] << " to " << vModules[nModules - 1] 
			<< endl
			<< setw(PRT_WIDTH) << "Modules count:" << setfill(SEPARATOR) << nModules << endl;
	
	return result;
}
