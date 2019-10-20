#include "hashmap.h"
#include <iostream>
#include <iomanip>

using namespace std;

#define CHK_PRT_SZ_CP_RESULT(ctor_name, b_result, i_status, size, capacity, exp1, exp2)\
{\
	string status = b_result ? " SUCCESS: " : " FAIL:     ";\
	i_status = !b_result; \
	cout 	<< left << setw(9)\
			<< status << ctor_name << setfill(' ')\
			<< "Map size = " << size\
			<< ", Map capacity = " << capacity\
			<< " : Expected values is (" << exp1 << ", " << exp2 << ")"\
	<< endl;\
}

int main() 
{
	typedef HashMap<int, int> 		HashMapInt;
	typedef HashMapInt::PairType 	PairType;
	
	bool 		bResult;
	int 		iStatus = 0; 
	size_t 		iSize, iCapacity;
	
	cout << "NOTE:    " << "Check default ctor" << endl;
	HashMapInt 	oHashMap1; // default ctor
	
	iSize = oHashMap1.size();
	iCapacity = oHashMap1.capacity();
	bResult = (iSize == 0 && iCapacity > 0);
	iStatus = (!bResult || iStatus);
	CHK_PRT_SZ_CP_RESULT("Default ctor: ", bResult, iStatus, iSize, iCapacity, 0, "> 0")
	
	cout << "NOTE:    " << "Check Initializer list ctor with 3 pairs" << endl;
	HashMapInt oHashMap2 { // initializer-list ctor  
		PairType(1,1), 
		PairType(2,2),
		PairType(3,3)
	}; 
	
	iSize = oHashMap2.size();
	iCapacity = oHashMap2.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	iStatus = (!bResult || iStatus);
	CHK_PRT_SZ_CP_RESULT("Test Initializer list ctor: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	cout << "NOTE:    " << "Check Copy ctor with copy previous map with 3 pairs" << endl;
	HashMapInt	oHashMap3(oHashMap2); // copy ctor
	
	iSize = oHashMap3.size();
	iCapacity = oHashMap3.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	iStatus = (!bResult || iStatus);
	CHK_PRT_SZ_CP_RESULT("Test Copy ctor: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	cout << "NOTE:    " << "Check clean() method" << endl;
	oHashMap2.clear();
	iSize = oHashMap2.size();
	iCapacity = oHashMap2.capacity();
	bResult = (iSize == 0 && iCapacity == 127);
	iStatus = (!bResult || iStatus);
	CHK_PRT_SZ_CP_RESULT("Test Source HashMap clear() method: ", bResult, iStatus, iSize, iCapacity, 0, 127)
	
	cout << "NOTE:    " << "Checking that the copy does not reference the source hash table" << endl;
	iSize = oHashMap3.size();
	iCapacity = oHashMap3.capacity();
	bResult = (iSize == 3 && iCapacity > 0);
	iStatus = (!bResult || iStatus);
	CHK_PRT_SZ_CP_RESULT("Test Copy after source clean: ", bResult, iStatus, iSize, iCapacity, 3, "> 0")
	
	return iStatus;
}
