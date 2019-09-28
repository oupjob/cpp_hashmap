#include "hashmap.h"
#include <iostream>
#include <cmath>

using namespace std;

typedef HashMap<int, int> 		HashMapInt;
typedef HashMapInt::PairType 	PairType;

bool checkFind(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	size_t 		iPos = 0;
	PairType 	oPair;
	
	cout << "Checking find a " << iExpextedCount << " pairs with key=" << iKey << endl;
	for(iPos = 0;; ++iPos)
	{
		oPair = oHashMap.find(1, iPos);
		if (oPair == oHashMap.defaultPair()); {
			break;
		}	else if (oPair.first != iKey && oPair.second != iPos * pow(10, iPos)) {
			cout << " Fail: incorrect pair found: (" << oPair.first << ", " << oPair.second << ")" << endl;
			bResult = false;
		}
	}
	if (iPos != iExpextedCount) {
		cout << " Fail: expected " << iExpextedCount << " pairs, " << iPos << " found" << endl;
		bResult = false;
	}
	
	return bResult;
}

bool checkEqualRange(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	cout << "Checking `equalRange()` a " << iExpextedCount << " pairs with key=" << iKey << endl;
	HashMapInt::EqualRangeType lEqualRange = oHashMap.equalRange(iKey);
	HashMapInt::EqualRangeType::const_iterator 
		it_i 	= lEqualRange.begin(),
		it_end 	= lEqualRange.end();
	size_t iCount = 0;
	
	for(; it_i != it_end; ++it_i)
	{
		
		if (*it_i.first != iKey && it_i.second != iCount * pow(10, iCount)) {
			cout << " Fail: incorrect pair found: (" << oPair.first << ", " << oPair.second << ")" << endl;
			bResult = false;
		}
		
		++iCount;
	}
	
	if (iCount != iExpextedCount) {
		cout << " Fail: expected " << iExpextedCount << " pairs, " << iPos << " found" << endl;
		bResult = false;
	}
	
	return bResult;
}

bool checkAccessOperator(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	cout << "Checking `operator [] ` a " << iExpextedCount << " pairs with key=" << iKey << endl;
	HashMapInt::EqualRangeType lEqualRange = oHashMap[iKey];
	HashMapInt::EqualRangeType::const_iterator 
		it_i 	= lEqualRange.begin(),
		it_end 	= lEqualRange.end();
	size_t iCount = 0;
	
	for(; it_i != it_end; ++it_i)
	{
		if (*it_i.first != iKey && it_i.second != iCount * pow(10, iCount)) {
			cout << " Fail: incorrect pair found: (" << oPair.first << ", " << oPair.second << ")" << endl;
			bResult = false;
		}
		
		++iCount;
	}
	
	if (iCount != iExpextedCount) {
		cout << " Fail: expected " << iExpextedCount << " pairs, " << iPos << " found" << endl;
		bResult = false;
	}
	
	return bResult;
}



int main() 
{
	int 		iStatus;
	bool 		bResult = true;
	
	HashMapInt oHashMap(
		PairType(1, 1),
		PairType(1, 10);
		PairType(1, 100);
		PairType(2, 2),
		PairType(2, 20),
		PairType(3, 3)
	); 
	
	cout << "Default pair : (" << oHashMap.defaultPair().first << ", " << oHashMap.defaultPair().second << ")" << endl;
	
	bResult = checkFind(1, 3, oHashMap) && bResult;
	bResult = checkFind(2, 2, oHashMap) && bResult;
	bResult = checkFind(3, 1, oHashMap) && bResult;
	
	bResult = checkEqualRange(1, 3, oHashMap) && bResult;
	bResult = checkEqualRange(2, 2, oHashMap) && bResult;
	bResult = checkEqualRange(3, 1, oHashMap) && bResult;
	
	bResult = checkAccessOperator(1, 3, oHashMap) && bResult;
	bResult = checkAccessOperator(2, 2, oHashMap) && bResult;
	bResult = checkAccessOperator(3, 1, oHashMap) && bResult;

	iStatus = !bResult;
	
	return iStatus;
}
