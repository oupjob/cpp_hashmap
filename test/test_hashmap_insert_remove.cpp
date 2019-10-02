#include "hashmap.h"
#include <iostream>
#include <cmath>

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

int main() 
{
	typedef HashMap<int, long long> HashMapIntLLong;
	typedef HashMapIntLLong::PairType 	PairType;
	
	int 				iStatus = 0;
	bool 				bResult;
	string 				sStatus;
	PairType 			oDefaultPair(123456789, 987654321), oPair, oExpectedPair;
	size_t				nRemovedCount, iPos;
	
	HashMapIntLLong 	oHashMap;
	
	
	cout << "NOTE:    " << "Checking inserts in empty map, then removes" << endl;
	
	for(int i = 1; i <= 4; ++i)
		oHashMap.insert(1, std::pow(10, i));
	
	bResult = (oHashMap.size() == 4);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after inserts in empty is ", oHashMap.size(), 3)
	
	oHashMap.remove(1, oDefaultPair, 5);
	bResult = (oHashMap.size() == 4);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after remove nonexistent element is ", oHashMap.size(), 3)
	
	bool bRemoveResult = true;
	nRemovedCount = 0;
	for(int i = 1; i <= 4; ++i) 
	{
		if (i % 2 == 0) {
			iPos = 0;
			oPair = oHashMap.remove(1, oDefaultPair, 4 - i);
			oExpectedPair = PairType(1, std::pow(10, 4 - i + 1));
		} else {
			iPos = 4 - i;
			oPair = oHashMap.remove(1, oDefaultPair, iPos);
			oExpectedPair = PairType(1, std::pow(10, 4 - i + 1));
		}
		
		bRemoveResult = (oPair == oExpectedPair) && bRemoveResult;
		if (!bRemoveResult) {
			PRT_PAIR_CHK_FAIL(oPair, oExpectedPair, iPos);
		} else {
			++nRemovedCount;
		}
			
	}
	bResult = bRemoveResult;
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", nRemovedCount, 4);
	
	bResult = (oHashMap.size() == 0);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after remove inserted by key in empty is ", oHashMap.size(), 0)
	
	cout << "NOTE:    " << "Checking many inserts, then removes (Default Pair is: " << COUT_PAIR(oDefaultPair) << ")"<< endl;
	
	for(int i = 0; i < 400; ++i) 
		for(int j = 0; j < 10; ++j) {
			oHashMap.insert(i, (i * 10 + j));
		}
	
	bResult = (oHashMap.size() == 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many inserts in non empty is ", oHashMap.size(), 4000);
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many inserts in non empty is ", oHashMap.capacity(), " '> 4000'");
	
	nRemovedCount = 0;
	bRemoveResult = true;
	for(int i = 0; i < 400; ++i)
	{		
		for(int j = 0; j < 10; ++j) 
		{
			if (j % 2 == 0) { // remove from begin
				iPos = 0;
				oPair = oHashMap.remove(i, oDefaultPair, iPos);
				oExpectedPair = PairType(i, (i * 10 + ((int)j/2)));
			} else {	// remove from end
				iPos = 10 - j - 1;
				oPair = oHashMap.remove(i, oDefaultPair, iPos);
				oExpectedPair = PairType(i, (i * 10 + (10 - ((int)j/2) - 1)));
			}
			bRemoveResult = (oPair == oExpectedPair);
			if (!bRemoveResult) {
				PRT_PAIR_CHK_FAIL(oPair, oExpectedPair, iPos);
			} else {
				++nRemovedCount;
			}
		}
	}
	
	bResult = bRemoveResult;
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Removed pairs count is: ", nRemovedCount, 4000);
		
	bResult = (oHashMap.size() == 0);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many removes is ", oHashMap.size(), 0);
	if (bResult)
		cout << "  NOTE:  " << "Many insert (with extend) operations did not corrupt insertion order" << endl;
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many removes is ", oHashMap.capacity(), " '> 4000'");
	
	return iStatus;
}
