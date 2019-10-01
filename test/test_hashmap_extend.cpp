#include "hashmap.h"
#include <iostream>

using namespace std;

#define PRT_RESULT(b_result, str_status, check_type, value, expected_value)\
str_status = b_result ? "SUCCESS: " : "FAIL   : "; \
cout 	<< str_status \
		<< check_type << value << ", " << expected_value << " expected" << endl;

int main() 
{
	typedef HashMap<int, int> 		HashMapInt;
	typedef HashMapInt::PairType 	PairType;
	
	int 		iStatus = 0;
	bool		bResult;
	std::string sStatus;
	
	HashMapInt oHashMap;
	
	size_t iCapacity = oHashMap.capacity();
	
	for(int i = 0; i < 16; ++i) {
		oHashMap.insert(1, 1);
		oHashMap.extend(iCapacity * (i + 2));
	}
	
	bResult = (oHashMap.capacity() == iCapacity * 17);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Hash Map capacity is: ", oHashMap.capacity(), iCapacity * 17)
			
	bResult = (oHashMap.size() == 16);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Hash Map size is: ", oHashMap.size(), 16)
	
	HashMapInt::EqualRangeType lEqualRange = oHashMap.equalRange(1);
	bResult = (lEqualRange.size() == 16);
	iStatus = (!bResult || !iStatus);
	PRT_RESULT(bResult, sStatus, "Pairs found: ", lEqualRange.size(), 16)
	
	;
	return iStatus;
}
