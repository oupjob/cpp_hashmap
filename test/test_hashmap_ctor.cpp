#include "hashmap.h"
#include <iostream>

using namespace std;

#define CHK_PRT_SZ_CP_RESULT(ctor_name, b_result, i_status, size, capacity, exp1, exp2)\
{\
	string status = b_result ? "Success: " : "Failed: ";\
	i_status = !b_result; \
	cout 	<< status << ctor_name << " " \
			<< "Map size = " << size\
			<< ", Map capacity = " << capacity\
			<< "Expected salues is (" << exp1 << ", " << exp2 << ")"\
	<< endl;\
}

int main() 
{
	typedef HashMap<int, int> 		HashMapInt;
	typedef HashMapInt::PairType 	PairType;
	
	HashMapInt 			oHashMap1; // default ctor
	HashMapInt	 		oHashMap2( // initializer-list ctor  
		PairType(1,1), 
		PairType(2,2),
		PairType(3,3)
	); 
	
	HashMapInt			oHashMap3(oHashMap2) // copy ctor
	
	int iStatus = 0, iSize, iCapacity;
	bool bResult;
	
	cout << "Check default ctor" << endl;
	iSize = oHashMap1.size();
	iCapacity = oHashMap1.capacity();
	bResult = (iSize == 0 && iCapacity > 0);
	CHK_PRT_SZ_CP_RESULT("Default ctor: ", bResult, iStatus, iSize, iCapacity, 0, "> 0")
	
	cout << "Check Initializer list ctor with 3 pairs" << endl;
	iSize = oHashMap2.size();
	iCapacity = oHashMap2.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	CHK_PRT_SZ_CP_RESULT("Initializer list ctor: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	cout << "Check Copy ctor with copy previous map with 3 pairs" << endl;
	iSize = oHashMap3.size();
	iCapacity = oHashMap3.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	CHK_PRT_SZ_CP_RESULT("Copy ctor: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	cout << "Check clean() method" << endl;
	oHashMap2.clear();
	iSize = oHashMap2.size();
	iCapacity = oHashMap2.capacity();
	bResult = (iSize == 0 && iCapacity == 0);
	CHK_PRT_SZ_CP_RESULT("Source HashMap of copy HashMap is cleaned: ", bResult, iStatus, iSize, iCapacity, 0, 0)
	
	cout << "Checking that the copy does not reference the source hash table" << endl;
	iSize = oHashMap3.size();
	iCapacity = oHashMap3.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	CHK_PRT_SZ_CP_RESULT("Copy after source clean: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	return iStatus;
}
