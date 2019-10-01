#include "hashmap.h"
#include <iostream>

using namespace std;

#define PRT_RESULT(b_result, str_status, check_type, value, expected_value)\
str_status = b_result ? "SUCCESS: " : "FAIL:    "; \
cout 	<< str_status \
		<< check_type << value << ", " << expected_value << " expected" << endl;

int main() 
{
	typedef HashMap<int, int> 		HashMapInt;
	typedef HashMapInt::PairType 	PairType;
	
	int 		iStatus = 0;
	bool 		bResult;
	string 		sStatus;
	HashMapInt::PairType oDefaultPair(1000000, 1000000);
	
	HashMapInt 	oHashMap;
	
	cout << "NOTE:    " << "Checking inserts in empty map, then removes" << endl;
	
	oHashMap.insert(1, 1);
	oHashMap.insert(1, 1);
	oHashMap.insert(1, 1);
	
	bResult = (oHashMap.size() == 3);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after inserts in empty is ", oHashMap.size(), 3)
	
	oHashMap.remove(1, oDefaultPair, 3);
	bResult = (oHashMap.size() == 3);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after remove nonexistent element is ", oHashMap.size(), 3)
	
	oHashMap.remove(1, oDefaultPair, 0);
	oHashMap.remove(1, oDefaultPair, 0);
	oHashMap.remove(1, oDefaultPair, 0);
	
	bResult = (oHashMap.size() == 0);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after remove inserted by key in empty is ", oHashMap.size(), 0)
	
	cout << "NOTE:    " << "Checking many inserts, then removes" << endl;
	
	for(int i = 0; i < 400; ++i) 
		for(int j = 1; j <= 10; ++j)
			oHashMap.insert(i * j, i * 10);
	
	bResult = (oHashMap.size() == 4000);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many inserts in non empty is ", oHashMap.size(), 4000);
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many inserts in non empty is ", oHashMap.capacity(), " '> 4000'");
	
	for(int i = 0; i < 400; ++i)
		for(int j = 1; j <= 10; ++j)
			oHashMap.remove(i * j, oDefaultPair, 0);
		
	bResult = (oHashMap.size() == 0);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Size of Hash Map after many removes is ", oHashMap.size(), 0);
	
	bResult = (oHashMap.capacity() > 4000);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Capacity of Hash Map after many removes is ", oHashMap.capacity(), " '> 4000'");
	
	return iStatus;
}
