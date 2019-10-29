#include "hashmap.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

#define PRT_RESULT(b_result, str_status, check_type, value, expected_value)\
str_status = b_result ? " SUCCESS: " : " FAIL:    "; \
cout 	<< str_status \
		<< check_type << value << ", " << expected_value << " expected" << endl;

#define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"

#define PRT_PAIR_CHK_FAIL(pair, expected_pair, pos) \
cout 	<< "  FAIL:    " \
		<< "Removed pair is:" << " (" << pair.first << ", " << pair.second << ")" \
		<< ", expected " << " (" << expected_pair.first << ", " << expected_pair.second << ")" \
		<< ", removed position is: " << pos \
		<< endl;
		
#define PRT_FIND_PAIR_CHK_FAIL(pair, pos) \
cout 	<< "  FAIL:    " \
		<< "Found removed pair:" << " (" << pair.first << ", " << pair.second << ")" \
		<< " in position " << pos \
		<< endl;

double my_log(double x, double base)
{
	return std::log(x) / std::log(base);
}
		
typedef HashMap<int, long long> HashMapIntLLong;
typedef HashMapIntLLong::PairType 	PairType;
typedef HashMapIntLLong::EqualRangeType EqualRangeType;
typedef HashMapIntLLong::ConstRefPairType ConstRefPairType;
		
size_t checkRemovedEqualRange(
	const EqualRangeType& 	lRemovedEqualRange,
	int 					iExpectedKey,
	int 					j,
	int						k,
	size_t 					nExpectedLen, 
	size_t 					iPos,
	const HashMapIntLLong&	oHashMap
)
{
	PairType oExpectedPair, oDefaultPair(12345678, 12345678);
	
	EqualRangeType::const_iterator 
		it_ch		= lRemovedEqualRange.begin(), 
		it_ch_end	= lRemovedEqualRange.end(),
		it_er;
		
	EqualRangeType 	lFoundEqualRange = oHashMap.equalRange(iExpectedKey, iPos, nExpectedLen), 
					lFoundEqualRangeOp = oHashMap[iExpectedKey];
		
	bool bEqResult, bResult = true;
	size_t nRemovedCount = 0;
	string sStatus;
	size_t iFindPos;
	
	for(; it_ch != it_ch_end; ++it_ch, ++k) 
	{
		oExpectedPair = PairType(iExpectedKey, iExpectedKey * 10 + k);
			
		HashMapIntLLong::SharedPtrPairType pChPair = it_ch->lock();
		
		bEqResult = (*pChPair == oExpectedPair);
		if (!bEqResult) {
			PRT_PAIR_CHK_FAIL((*pChPair), oExpectedPair, iPos);
			bResult = false;
		}
		
		// check find removed elements in hash map
		iFindPos = 0;
		while(true) {
			HashMapIntLLong::ConstRefPairType oPair = oHashMap.find(iExpectedKey, oDefaultPair, iFindPos++);
			bResult = (oPair != (*pChPair));
			if (!bResult)
				PRT_FIND_PAIR_CHK_FAIL(oPair, (iPos + nRemovedCount));
			
			if (oPair == oDefaultPair)
				break;
		}
		
		auto chkWeakPtrPair = [&pChPair](const HashMapIntLLong::WeakPtrPairType& pPair)
		{
			HashMapIntLLong::SharedPtrPairType spPair = pPair.lock();
			return ((*pChPair) == (*spPair));
		};
		
		it_er = std::find_if(lFoundEqualRange.begin(), lFoundEqualRange.end(), chkWeakPtrPair);
		if (it_er != lFoundEqualRange.end()) {
			HashMapIntLLong::SharedPtrPairType pErPair = it_er->lock();
			PRT_FIND_PAIR_CHK_FAIL((*pErPair), (iPos + nRemovedCount))
		}
		
		it_er = std::find_if(lFoundEqualRangeOp.begin(), lFoundEqualRangeOp.end(), chkWeakPtrPair);
		if (it_er != lFoundEqualRangeOp.end()) {
			HashMapIntLLong::SharedPtrPairType pEsrPair = it_er->lock();
			PRT_FIND_PAIR_CHK_FAIL((*pEsrPair), (iPos + nRemovedCount))
		}
		
		++nRemovedCount;
	}
	
	bResult = (nRemovedCount == nExpectedLen);
	if (!bResult) {
		PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", nRemovedCount, nExpectedLen);
	}
	
	return nRemovedCount;
}


		
int main() 
{
	int 				iStatus = 0;
	bool 				bResult, bRemoveResult;
	string 				sStatus;
	size_t				nRemovedCount, iPos;
	EqualRangeType		lRemovedEqualRange;
	HashMapIntLLong 	oHashMap;
	size_t 				nExpectedLen;
	
	cout << "NOTE:    " << "Checking many inserts, then removes half of hash map" << endl;
	
	for(int i = 0; i < 400; ++i) 
	{
		for(int j = 0; j < 10; ++j) 
		{
			if (i % 2) {
				oHashMap.insert(i, (i * 10 + j));
			} else {
				oHashMap.insert(PairType(i, i * 10 + j));
			}
		}
	}
	
	bResult = (oHashMap.size() == 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many inserts is ", oHashMap.size(), 4000);
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many inserts is ", oHashMap.capacity(), " '> 4000'");
	
	nRemovedCount = 0;
	bRemoveResult = true;
	int vk[] = {0, 9, 4, 8, 6, 7 }; // j => k
	int vPos[] = { 0, 5, 0, 2, 0, 0 };
	for(int i = 0; i < 200; ++i)
	{		
		for(int j = 0; j < 6; ++j) 
		{
			if (j % 2 == 0) { // remove from begin
				iPos = 0;
				nExpectedLen = (j == 4) ? 1 : 4 - j;
			} else {	// remove from end
				lRemovedEqualRange = oHashMap.remove(i, 10, 10);
				
				bRemoveResult = (lRemovedEqualRange.size() == 0);
				if (!bRemoveResult) {
					PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", lRemovedEqualRange.size(), 0);
				}
				
				nExpectedLen = 1;
			}
			
			lRemovedEqualRange = oHashMap.remove(i, vPos[j], nExpectedLen);
			nRemovedCount += checkRemovedEqualRange(lRemovedEqualRange, i, j, vk[j], nExpectedLen, vPos[j], oHashMap);
		}
	}
	
	bResult = (nRemovedCount == 2000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", nRemovedCount, 2000);
	
	bResult = (oHashMap.size() == 2000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many removes is ", oHashMap.size(), 2000);
	if (bResult)
		cout << "  NOTE:  " << "Many insert (with extend) operations did not corrupt insertion order" << endl;
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many removes is ", oHashMap.capacity(), " '> 4000'");
	
	cout << "NOTE:    " << "Restore half of removed pairs, then remove all" << endl;

	for(int i = 0; i < 200; ++i)
	{		
		for(int j = 0; j < 10; ++j) 
		{
			if (i % 2) {
				oHashMap.insert(i, (i * 10 + j));
			} else {
				oHashMap.insert(PairType(i, i * 10 + j));
			}
		}
	}
	
	bResult = (oHashMap.size() == 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many inserts is ", oHashMap.size(), 4000);
	
	bResult = (oHashMap.capacity() < 4100);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many inserts is ", oHashMap.capacity(), " '< 4100'");
	
	
	nRemovedCount = 0;
	bRemoveResult = true;
	for(int i = 0; i < 400; ++i)
	{		
		for(int j = 0; j < 6; ++j) 
		{
			if (j % 2 == 0) { // remove from begin
				iPos = 0;
				nExpectedLen = (j == 4) ? 1 : 4 - j;
			} else {	// remove from end
				lRemovedEqualRange = oHashMap.remove(i, 10, 10);
				
				bRemoveResult = (lRemovedEqualRange.size() == 0);
				if (!bRemoveResult) {
					PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", lRemovedEqualRange.size(), 0);
				}
				
				nExpectedLen = 1;
			}
			
			lRemovedEqualRange = oHashMap.remove(i, vPos[j], nExpectedLen);
			nRemovedCount += checkRemovedEqualRange(lRemovedEqualRange, i, j, vk[j], nExpectedLen, vPos[j], oHashMap);
		}
	}
	
	bResult = (nRemovedCount == 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", nRemovedCount, 4000);
	
	bResult = (oHashMap.size() == 0);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many removes is ", oHashMap.size(), 0);
	if (bResult)
		cout << "  NOTE:  " << "Many insert (with extend) operations did not corrupt insertion order" << endl;
	
	bResult = (oHashMap.capacity() > 4000 && oHashMap.capacity() < 4100);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many removes is ", oHashMap.capacity(), " '> 4000 and < 4100'");
	
	return iStatus;
}
