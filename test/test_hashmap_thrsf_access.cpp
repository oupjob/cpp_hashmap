// #include "hashmap.h"
// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <chrono>
// #include <ctime>
// #include <iomanip>
// #include <exception>
// #include <random>
// 
// #include "thrsf_wrappers.h"
// 
// using namespace std;
// 
// typedef HashMap<int, int> 					HashMapInt;
// typedef HashMapInt::PairType 				PairType;
// typedef HashMapInt::ConstRefPairType		ConstRefPairType;
// typedef HashMapInt::EqualRangeType 			EqualRangeType;
// typedef HashMapInt::ConstRefWeakPtrPairType	ConstRefWeakPtrPairType;
// 
// typedef std::lock_guard<std::mutex> 	LockGuard;
// typedef std::list<std::thread>			ThreadList;
// 
// #define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"
// 
// bool checkInsertAndFindThreads(
// 	HashMapInt& oHashMap, TSOperationTimingVector& vTSOpTimings, int iStartKey, int iEndKey, size_t nCount
// ) {
// 	ThreadList lThreads;
// 	std::random_device oRandom;
// 	HashMapInt::PairType oDefaultPair(12345678, 12345678);
// 	size_t iPos = 0, nThreadCount = 0, iPrevPos;
// 
// 	// run
// 	for(int iKey = iStartKey; iKey <= iEndKey; ++iKey) 
// 	{
// 		while(iPos < nCount)
// 		{
// 			if (oRandom() % 2) {
// 				lThreads.push_back(std::thread(insertHashMapWorker, std::ref(oHashMap), iKey, iKey * 10 + iPos, std::ref(vTSOpTimings)));
// 				if (iPos) {
// 					iPrevPos = iPos - 1;
// 					lThreads.push_back(
// 						std::thread(findHashMapWorker, std::ref(oHashMap),iKey, iPrevPos, std::ref(vTSOpTimings))
// 					);
// 					lThreads.push_back(
// 						std::thread(equalRangeHashMapWorker, std::ref(oHashMap),iKey, iPrevPos, nCount, std::ref(vTSOpTimings))
// 					);
// 					lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap),iKey, std::ref(vTSOpTimings)));
// 		
// 					nThreadCount += 3;
// 				}
// 				++nThreadCount;
// 				++iPos;
// 			} else {
// 				lThreads.push_back(std::thread(findHashMapWorker, std::ref(oHashMap),iKey, iPos, std::ref(vTSOpTimings)));
// 				lThreads.push_back(std::thread(equalRangeHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount, std::ref(vTSOpTimings)));
// 				lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap), iKey, std::ref(vTSOpTimings)));
// 				nThreadCount += 3;
// 			}
// 		}
// 	}
// 	ThreadList::iterator it = lThreads.begin(), it_end = lThreads.end();
// 	for(; it != it_end; ++it) 
// 		(*it).join();
// 	
// 	// check
// 	vTSOpTimings.sortByEndTimeAscAndCalcLockTimes();
// 	bool bResult = true, bFindBeforeResult, bFindAfterResult, bInserterHaveFindCallersAfter, bErResult, bErqResult;
// 	
// 	TSOperationTimingVector vInserters = vTSOpTimings.filterByType(INSERT);
// 	size_t nInserters = vInserters.size();
// 	TSOperationTimingVector vParallels;
// 	
// 	vInserters.dump();
// 	
// 	for(size_t i = 0; i < nInserters; ++i)
// 	{
// 		bInserterHaveFindCallersAfter = bFindBeforeResult = bFindAfterResult = bErResult = bErqResult = true;
// 		
// 		const InsertOperationTiming* pInsertOperationTiming = static_cast<const InsertOperationTiming*>(vInserters[i]);
// 		vParallels = vTSOpTimings.filterParallelWorkersByType(pInsertOperationTiming, FIND_OPS);
// 		size_t nParallels = vParallels.size();
// 		
//  		vParallels.dump();
// 		
// 		for(size_t j = 0; j < nParallels; ++j)
// 		{
// 			// parallel find started rather than insert
// 			if (vParallels[j]->m_kOperationType & FIND) {
// 				const FindOperationTiming* pFindOperationTiming = static_cast<const FindOperationTiming*>(vParallels[j]);
// 				if (pFindOperationTiming->m_tLockTime > pInsertOperationTiming->m_tLockTime) {
// 					bFindBeforeResult = pFindOperationTiming->m_oResult.second != pInsertOperationTiming->m_tValue && bFindBeforeResult;
// 				} else if (!bFindAfterResult) {
// 					bInserterHaveFindCallersAfter = true;
// 					bFindAfterResult = pFindOperationTiming->m_oResult.second == pInsertOperationTiming->m_tValue && bFindBeforeResult;
// 				}
// 			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
// 				const EqualRangeOperationTiming* pEqualRangeOperationTiming = static_cast<const EqualRangeOperationTiming*>(vParallels[j]);
// 				
// 				EqualRangeType::const_iterator 
// 					it = pEqualRangeOperationTiming->m_lResult.begin(),
// 					it_end = pEqualRangeOperationTiming->m_lResult.end();
// 					
// 				it = std::find_if(
// 					it, 
// 					it_end, 
// 					[pInsertOperationTiming](ConstRefWeakPtrPairType pWeakPtrToPair) {
// 						return pWeakPtrToPair.lock()->second == pInsertOperationTiming->m_tValue;
// 					}
// 				);
// 				
// 				bErResult = it != it_end;
// 			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
// 				const EqualRangeQOperationTiming* 
// 					pEqualRangeQOperationTiming = static_cast<const EqualRangeQOperationTiming*>(vParallels[j]);
// 				
// 				EqualRangeType::const_iterator 
// 					it = pEqualRangeQOperationTiming->m_lResult.begin(),
// 					it_end = pEqualRangeQOperationTiming->m_lResult.end();
// 					
// 				it = std::find_if(
// 					it, 
// 					it_end, 
// 					[pInsertOperationTiming](ConstRefWeakPtrPairType pWeakPtrToPair) {
// 						return pWeakPtrToPair.lock()->second == pInsertOperationTiming->m_tValue;
// 					}
// 				);
// 				bErqResult = it != it_end;
// 			}
// 		}
// 
// 		if (!bFindBeforeResult)
// 			cout 	<< " FAIL:     " 
// 					<< "Inserted Pair: (" << pInsertOperationTiming->m_tKey << ", " << pInsertOperationTiming->m_tValue 
// 					<< ") found before insertion by `find`"
// 			<< endl;
// 		
// 		if (bInserterHaveFindCallersAfter && !bFindAfterResult)
// 			cout 	<< " FAIL:     " 
// 					<< "Inserted Pair: (" << pInsertOperationTiming->m_tKey << ", " << pInsertOperationTiming->m_tValue 
// 					<< ") not found after insertion by `find`"
// 			<< endl;
// 		
// 		if (!bErResult)
// 			cout 	<< " FAIL:     " 
// 					<< "Inserted Pair: (" << pInsertOperationTiming->m_tKey << ", " << pInsertOperationTiming->m_tValue 
// 					<< ") not found after insertion by `equalRange`"
// 			<< endl;
// 		
// 		if (!bErqResult)
// 			cout 	<< " FAIL:     " 
// 					<< "Inserted Pair: (" << pInsertOperationTiming->m_tKey << ", " << pInsertOperationTiming->m_tValue 
// 					<< ") not found after insertion by `operator []`"
// 			<< endl;
// 		
// 		bResult = bResult && bFindBeforeResult && bFindAfterResult && bErResult && bErqResult;
// 	}
// 	
// 	vTSOpTimings.clear();
// 	
// 	return bResult;
// }
// 
// int main() 
// {	
// 	HashMapInt 			oHashMap;
// 	
// 	int 		iStatus = 0;
// 	bool 		bResult;
// 	size_t		nThreadCount;
// 	
// 	TSOperationTimingVector vTsOpTimings;
// 	
// 	nThreadCount = checkInsertAndFindThreads(oHashMap, vTsOpTimings, 0, 10, 10);
// 	
// 	return iStatus;
// }

int main()
{
	return 0;
}
