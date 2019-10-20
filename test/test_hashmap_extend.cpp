// #include "hashmap.h"
// #include <iostream>
// 
// using namespace std;
// 
// #define PRT_RESULT(b_result, str_status, check_type, value, expected_value)\
// str_status = b_result ? "SUCCESS: " : "FAIL   : "; \
// cout 	<< str_status \
// 		<< check_type << value << ", " << expected_value << " expected" << endl;
// 
// 		
// #define PRT_PAIR_CHK_FAIL(pair, expected_pair, iPos) \
// cout 	<< "  FAIL:    " \
// 		<< "Found pair:" << " (" << pair.first << ", " << pair.second << ")" \
// 		<< ", expected " << " (" << expected_pair.first << ", " << expected_pair.second << ")" \
// 		<< ", find iPosition is: " << iPos \
// 		<< endl;
// 		
// int main() 
// {
// 	typedef HashMap<int, int> 		HashMapInt;
// 	typedef HashMapInt::PairType 	PairType;
// 	
// 	int 		iStatus = 0;
// 	bool		bResult;
// 	std::string sStatus;
// 	
// 	HashMapInt oHashMap, oNonUsedHashMap;
// 	
// 	size_t iCapacity = oHashMap.capacity();
// 	
// 	
// 	oNonUsedHashMap.extend(iCapacity * 2);
// 	bResult = (oNonUsedHashMap.capacity() == iCapacity * 2);
// 	iStatus = (!bResult || iStatus);
// 	PRT_RESULT(bResult, sStatus, "Extended Empty Hash Map capacity is: ", oNonUsedHashMap.capacity(), iCapacity * 2)
// 	
// 	
// 	for(int i = 0; i < 16; ++i) {
// 		oHashMap.insert(1, 1);
// 		oHashMap.extend(iCapacity * (i + 2));
// 	}
// 	
// 	bResult = (oHashMap.capacity() == iCapacity * 17);
// 	iStatus = (!bResult || iStatus);
// 	PRT_RESULT(bResult, sStatus, "Hash Map capacity is: ", oHashMap.capacity(), iCapacity * 17)
// 			
// 	bResult = (oHashMap.size() == 16);
// 	iStatus = (!bResult || iStatus);
// 	PRT_RESULT(bResult, sStatus, "Hash Map size is: ", oHashMap.size(), 16)
// 	
// 	HashMapInt::EqualRangeType lEqualRange = oHashMap.equalRange(1);
// 	bResult = (lEqualRange.size() == 16);
// 	iStatus = (!bResult || iStatus);
// 	PRT_RESULT(bResult, sStatus, "Pairs found: ", lEqualRange.size(), 16)
// 	
// 	
// 	HashMapInt		oHashMap2;
// 	for(int i = 0; i < 400; ++i) 
// 		for(int j = 0; j < 10; ++j)
// 			oHashMap2.insert(i, (i * 10 + j));
// 
// 	bool bFindResult = true;
// 	size_t iPos;
// 	PairType oPair, oDefaultPair(123456789, 987654321), oExpectedPair;
// 	HashMapInt::EqualRangeType::iterator it_er, it_end;
// 	for(int i = 0; i < 400; ++i)
// 	{
// 		for(int j = 0; j < 10; ++j) 
// 		{
// 			oExpectedPair = PairType(i, (i * 10 + j));
// 			oPair = oHashMap2.find(i, oDefaultPair, j);
// 			
// 			bFindResult = (oPair == oExpectedPair);
// 			if (!bFindResult)
// 				PRT_PAIR_CHK_FAIL(oPair, oExpectedPair, j);
// 		}
// 		bResult = bFindResult && bResult;
// 		
// 		lEqualRange = oHashMap2.equalRange(i);
// 		it_end = lEqualRange.end();
// 		for(it_er = lEqualRange.begin(), iPos = 0; it_er != lEqualRange.end(); ++it_er, ++iPos)
// 		{
// 			oExpectedPair = PairType(i, (i * 10 + iPos));
// 			bFindResult = (**it_er == oExpectedPair);
// 			if (!bFindResult)
// 				PRT_PAIR_CHK_FAIL((**it_er), oExpectedPair, iPos);
// 		}
// 		bResult = bFindResult && bResult;
// 		
//  		lEqualRange = oHashMap2[i];
// 		it_end = lEqualRange.end();
// 		for(it_er = lEqualRange.begin(), iPos = 0; it_er != lEqualRange.end(); ++it_er, ++iPos)
// 		{
// 			oExpectedPair = PairType(i, (i * 10 + iPos));
// 			bFindResult = (**it_er == oExpectedPair);
// 			if (!bFindResult)
// 				PRT_PAIR_CHK_FAIL((**it_er), oExpectedPair, iPos);
// 		}
// 		bResult = bFindResult && bResult;
// 	}
// 	
// 	iStatus = (!bResult || iStatus);
// 	if (bResult)
// 		cout << "SUCCESS: " << "Many extend of full map operations did not corrupt insertion order" << endl;
// 	else
// 		cout << "FAIL   : " << "Many extend of full map operations did corrupt insertion order" << endl;
// 	
// 	cout <<  "REM:     Full check of extend with many inserts coveraged in `test_hashmap_insert_remove`" << endl;
// 	
// 	return iStatus;
// }

int main() {
	return 0;
}
