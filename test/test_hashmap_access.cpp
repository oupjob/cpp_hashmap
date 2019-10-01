#include "hashmap.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

// hash map extends the hash table adding a default capacity value (128)
#define DEFAULT_IK 0

// set of collisions are easy to identify for this function
hash_t badHashFunction(size_t iModulo, int iKey, size_t ik = DEFAULT_IK) {
	hash_t h = 7 * iKey + 23 + ik;
	if (!h) h = 1;
	return h % iModulo;
}

#undef DEFAULT_IK

typedef HashMap<int, int, badHashFunction> 	HashMapInt;
typedef HashMapInt::PairType 				PairType;

#define OUT_MSG_PREFIX(func) "Checking `" func "` key=" << iKey << ": "

bool checkFind(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	size_t 		iPos = 0;
	
	HashMapInt::PairType oDefaultPair(0, 0);
	
	for(iPos = 0;; ++iPos)
	{
		const PairType& oPair = oHashMap.find(iKey, oDefaultPair, iPos);
		
		if (oPair.first == oDefaultPair.first) {
			break;
		} else if (oPair.first != iKey) {
			cout 	<< "FAIL:    " << OUT_MSG_PREFIX("find()") 
					<< "incorrect pair found: (" << oPair.first << ", " << oPair.second << ")" << endl;
			bResult = false;
		}
	}
	
	if (iPos == (oHashMap.size() - 1)) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("find()") 
				<< " expected " << iExpextedCount << " or more pairs, Hash Map size = " << iPos << " found" << endl;
		return false;
	}
	
	if (iPos < iExpextedCount) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("find()") 
				<< "expected " << iExpextedCount << " or more pairs, " << iPos << " found" << endl;
		bResult = false;
	} else {
		cout 	<< "SUCCESS: " << OUT_MSG_PREFIX("find()")  
				<< "expected " << iExpextedCount << " or more pairs, " << iPos << " found" << endl;
		bResult = true;
	}
	
	return bResult;
}

bool checkEqualRange(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	HashMapInt::EqualRangeType lEqualRange = oHashMap.equalRange(iKey);
	HashMapInt::EqualRangeType::const_iterator 
		it_i 	= lEqualRange.begin(),
		it_end 	= lEqualRange.end();
	size_t iCount = 0;
	
	for(; it_i != it_end; ++it_i)
	{
		if (it_i->first != iKey) {
			cout 	<< "FAIL: 	" << OUT_MSG_PREFIX("equalRange()") 
					<< " incorrect pair found: (" << it_i->first << ", " << it_i->second << ")" << endl;
			bResult = false;
		}
		
		++iCount;
	}
	
	if (iCount == (oHashMap.size() - 1)) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("equalRange()") 
				<< " expected " << iExpextedCount << " or more pairs, Hash Map size = " << iCount << " found" << endl;
		return false;
	}
	
	if (iCount < iExpextedCount) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("equalRange()") 
				<< "expected " << iExpextedCount << " or more pairs, " << iCount << " found" << endl;
		bResult = false;
	} else {
		cout 	<< "SUCCESS: " << OUT_MSG_PREFIX("equalRange()")  
				<< "expected " << iExpextedCount << " or more pairs, " << iCount << " found" << endl;
		bResult = true;
	}
	
	return bResult;
}

bool checkAccessOperator(int iKey, size_t iExpextedCount, const HashMapInt& oHashMap)
{
	bool 		bResult = true;
	HashMapInt::EqualRangeType lEqualRange = oHashMap[iKey];
	HashMapInt::EqualRangeType::const_iterator 
		it_i 	= lEqualRange.begin(),
		it_end 	= lEqualRange.end();
	size_t iCount = 0;
	
	for(; it_i != it_end; ++it_i)
	{
		if (it_i->first != iKey) {
			cout << "FAIL:     incorrect pair found: (" << it_i->first << ", " << it_i->second << ")" << endl;
			bResult = false;
		}
		
		++iCount;
	}
	
	if (iCount == (oHashMap.size() - 1)) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("operator []") 
				<< " expected " << iExpextedCount << " or more pairs, Hash Map size = " << iCount << " found" << endl;
		return false;
	}
	
	if (iCount < iExpextedCount) {
		cout 	<< "FAIL:    " << OUT_MSG_PREFIX("operator []") 
				<< "expected " << iExpextedCount << " or more pairs, " << iCount << " found" << endl;
		bResult = false;
	} else {
		cout 	<< "SUCCESS: " << OUT_MSG_PREFIX("operator []")  
				<< "expected " << iExpextedCount << " or more pairs, " << iCount << " found" << endl;
		bResult = true;
	}
	
	return bResult;
}



int collisionForKey(int iKey, size_t iModulo, int i) {
	return iKey + i * iModulo;
}

#define DEFAULT_CAPACITY 128

void insertKeysWithCollisionsAndFillInHashMap(HashMapInt& rHashMap, int iCollisionedKey, size_t nKeyInsertCount, size_t nFillCount)
{	
	size_t 	iExtendCount = std::ceil((nFillCount + rHashMap.capacity()) / DEFAULT_CAPACITY),
			iNewCapacity = DEFAULT_CAPACITY * iExtendCount,
			n_inserts = nKeyInsertCount * 4;
			
	// insert keys into middle of collisions
			
	std::vector<int> vKeys;
	std::random_device oRandom;
	vKeys.reserve(n_inserts);

	for(int i = 0; i < (int)nKeyInsertCount; ++i) {
		vKeys.push_back(iCollisionedKey);
	}
	for(int i = 1; i <= (int)(n_inserts - nKeyInsertCount); ++i) {
		vKeys.push_back(collisionForKey(iCollisionedKey, iNewCapacity, i));
	}
	
	std::random_shuffle(vKeys.begin(), vKeys.end());
	
	if (vKeys[0] == iCollisionedKey) {
		vKeys[0] = vKeys[1];
		vKeys[1] = iCollisionedKey;
	}
	
	for(size_t i = 0, j = 0; i < n_inserts; ++i) {
		if (vKeys[i] == iCollisionedKey)
			++j;
		
		hash_t 	h = badHashFunction(iNewCapacity, iCollisionedKey), 
				hk = badHashFunction(iNewCapacity, vKeys[i]);
		rHashMap.insert(vKeys[i], vKeys[i] * 100 + j);
		
	}
	// fill width random keys
// 	nFillCount -= (n_inserts + nKeyInsertCount);
	for(size_t i = 0; i < nFillCount; ++i) {
		rHashMap.insert(oRandom(), oRandom());
	}
}

#undef DEFAULT_CAPACITY

int main() 
{
	int 		iStatus;
	bool 		bResult = true;
	
	HashMapInt oHashMap;
	
	oHashMap.insert(2, 2);
	oHashMap.insert(2, 2);
	oHashMap.insert(2, 2);
	oHashMap.insert(2, 2);
	
	cout << "NOTE:    " << "Check find after inserts in empty map" << endl;
	
	bResult = checkFind(2, 4, oHashMap) && bResult;
	bResult = checkEqualRange(2, 4, oHashMap) && bResult;
	bResult = checkAccessOperator(2, 4, oHashMap) && bResult;
	
	insertKeysWithCollisionsAndFillInHashMap(oHashMap, 10, 5, 200);
	insertKeysWithCollisionsAndFillInHashMap(oHashMap, 20, 3, 500);
	insertKeysWithCollisionsAndFillInHashMap(oHashMap, 30, 8, 300);
	insertKeysWithCollisionsAndFillInHashMap(oHashMap, 1, 32, 300);
	
	cout << "NOTE:    " << "Check find after insert and extend in non empty map" << endl;
	
	bResult = checkFind(10, 5, oHashMap) && bResult;
	bResult = checkFind(20, 3, oHashMap) && bResult;
	bResult = checkFind(30, 7, oHashMap) && bResult;
	bResult = checkFind(1, 32, oHashMap) && bResult;
// 	
	bResult = checkEqualRange(10, 5, oHashMap) && bResult;
	bResult = checkEqualRange(20, 3, oHashMap) && bResult;
	bResult = checkEqualRange(30, 7, oHashMap) && bResult;
	bResult = checkEqualRange(1, 32, oHashMap) && bResult;
	
	bResult = checkAccessOperator(10, 5, oHashMap) && bResult;
	bResult = checkAccessOperator(20, 3, oHashMap) && bResult;
	bResult = checkAccessOperator(30, 7, oHashMap) && bResult;
	bResult = checkAccessOperator(1, 32, oHashMap) && bResult;
	
	iStatus = !bResult;
	
	return iStatus;
}
