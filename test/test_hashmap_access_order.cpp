#include "hashmap.h"
#include <iostream>

#define PRT_PAIR_CHK_FAIL(pair, expected_pair, iPos) \
cout 	<< "  FAIL:    " \
		<< "Found pair:" << " (" << pair.first << ", " << pair.second << ")" \
		<< ", expected " << " (" << expected_pair.first << ", " << expected_pair.second << ")" \
		<< ", find iPosition is: " << iPos \
		<< endl;

		
#define PRT_RESULT_FAIL(msg, key, value, expected_value) \
cout 	<< " FAIL:    " << msg << " (key=" << key << ") : " \
		<< "expected " << expected_value << " or more pairs, " << value << " found" << endl;
		
		
		
using namespace std;

typedef HashMap<int, int> 		HashMapInt;
typedef HashMapInt::PairType 	PairType;

int main()
{
	HashMapInt		oHashMap;
	size_t			iPos, nCount, nExpectedCount;
	bool bResult = true, bFindResult = true;
	PairType oPair, oDefaultPair(123456789, 987654321), oExpectedPair;
	
	HashMapInt::EqualRangeType lEqualRange, lEqualSubRange;
	HashMapInt::EqualRangeType::const_iterator it_er, it_er_end, it_sr, it_sr_end;
	
	for(int i = 0; i < 400; ++i) 
		for(int j = 0; j < 10; ++j)
			oHashMap.insert(i, (i * 10 + j));
		
	for(int i = 0; i < 400; ++i)
	{
		for(int j = 0; j < 10; ++j) 
		{
			oExpectedPair = PairType(i, (i * 10 + j));
			oPair = oHashMap.find(i, oDefaultPair, j);
			
			bResult = (oPair == oExpectedPair);
			if (!bResult)
				PRT_PAIR_CHK_FAIL(oPair, oExpectedPair, j);
		}
		
		lEqualRange = oHashMap.equalRange(i);
		it_er_end = lEqualRange.end();
		for(it_er = lEqualRange.begin(), iPos = 0; it_er != lEqualRange.end(); ++it_er, ++iPos)
		{
			HashMapInt::SharedPtrPairType pErPair = it_er->lock();
			oExpectedPair = PairType(i, (i * 10 + iPos));
			
			bResult = (*pErPair == oExpectedPair);
			if (!bResult)
				PRT_PAIR_CHK_FAIL((*pErPair), oExpectedPair, iPos);
			
			// checking subreange
			nExpectedCount = (10 - iPos >= 4) ? 4 : 10 - iPos;
			lEqualSubRange = oHashMap.equalRange(i, iPos, nExpectedCount);
			if (nExpectedCount != lEqualSubRange.size()) {
				PRT_RESULT_FAIL("Checking equalRange subrange size ", i, lEqualSubRange.size(), nExpectedCount)
				continue;
			}
			
			size_t k = iPos;
			for(
				it_sr = lEqualSubRange.begin(), it_sr_end = lEqualSubRange.end(); 
				it_sr != it_sr_end; 
				++it_sr, ++k
			) {
				
				oExpectedPair = PairType(i, (i * 10 + k));
				HashMapInt::SharedPtrPairType pEsrPair = it_sr->lock();
				
				bFindResult = ((*pEsrPair) == oExpectedPair);
				
				if (!bFindResult)
					PRT_PAIR_CHK_FAIL((*pEsrPair), oExpectedPair, k);
			}
			
		}
		bResult = bFindResult && bResult;
		
 		lEqualRange = oHashMap[i];
		it_er_end = lEqualRange.end();
		for(it_er = lEqualRange.begin(), iPos = 0; it_er != lEqualRange.end(); ++it_er, ++iPos)
		{
			oExpectedPair = PairType(i, (i * 10 + iPos));
			HashMapInt::SharedPtrPairType pErPair = it_er->lock();
			
			bResult = (*pErPair == oExpectedPair);
			if (!bResult)
				PRT_PAIR_CHK_FAIL((*pErPair), oExpectedPair, iPos);
		}
	}
	
	if (bResult)
		cout << "SUCCESS: " << "Find operations results is match insertion order" << endl;
	else
		cout << "FAIL   : " << "Find operations results is not match insertion order" << endl;
	
	return !bResult;
}
