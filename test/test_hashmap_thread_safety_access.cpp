#include "hashmap.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <exception>
#include <random>

#include "thread_safety_testable_hashmap.h"

using namespace std;

typedef ThreadSafetyTestableHashMap<stdHashFunction> 	TSTHashMapInt;
typedef TSTHashMapInt::PairType 						PairType;
typedef TSTHashMapInt::ConstRefPairType					ConstRefPairType;
typedef TSTHashMapInt::EqualRangeType 					EqualRangeType;
typedef TSTHashMapInt::ConstRefWeakPtrPairType			ConstRefWeakPtrPairType;
typedef TSTHashMapInt::ConstSharedPtrPairType			ConstSharedPtrPairType;

typedef std::lock_guard<std::mutex> 	LockGuard;
typedef std::list<std::thread>			ThreadList;

#define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"

#define PRT_INS_FIND_BEFORE_RESULT_FAIL(b_flag, ins_timing, method)\
if (!b_flag)\
	cout 	<< " FAIL:     " \
			<< "Inserted Pair: (" << ins_timing->m_tKey << ", " << ins_timing->m_tValue \
			<< ") found before insertion by `" << method << "`" << endl;
		
#define PRT_INS_FIND_AFTER_RESULT_FAIL(b_flag, ins_timing, method)\
if (!b_flag)\
	cout 	<< " FAIL:     " \
			<< "Inserted Pair: (" << ins_timing->m_tKey << ", " << ins_timing->m_tValue \
			<< ") not found after insertion by `" << method << "`" << endl;

// bool checkInsertAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
// {
// 	ThreadList lThreads;
// 	std::random_device oRandom;
// 	TSTHashMapInt::PairType oDefaultPair(12345678, 12345678);
// 	
// 	// run
// 	for(int iKey = iStartKey; iKey <= iEndKey; ++iKey) 
// 	{
// 		size_t iPos = 0, iPrevPos;
// 		bool bEnd = false;
// 		while(iPos < nCount)
// 		{
// 			iPrevPos = (iPos) ? iPos - 1 : iPos;
// 			lThreads.push_back(
// 				std::thread(findHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount)
// 			);
// 			lThreads.push_back(
// 				std::thread(equalRangeHashMapWorker, std::ref(oHashMap),iKey, iPos, nCount)
// 			);
// 			lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
// 			
// 			lThreads.push_back(std::thread(insertHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
// 			
// 			lThreads.push_back(std::thread(findHashMapWorker, std::ref(oHashMap),iKey, iPos, nCount));
// 			lThreads.push_back(std::thread(equalRangeHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
// 			lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
// 			
// 			++iPos;
// 		}
// 	}
// 	ThreadList::iterator it = lThreads.begin(), it_end = lThreads.end();
// 	for(; it != it_end; ++it) 
// 		(*it).join();
// 	
// 	// check
// 	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
// 	
// 	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
// 	bool 	bFindBeforeResult, bFindAfterResult, 
// 			bErBeforeResult, bErAfterResult, 
// 			bErqBeforeResult, bErqAfterResult, 
// 			bHasFindOpAfter, bHasErOpAfter, bHasErqOpAfter;
// 	
// 	TSOperationTimingVector vInserters = oHashMap.m_vTSOpTimings.filterByType(INSERT);
// 	size_t nInserters = vInserters.size();
// 	TSOperationTimingVector vParallels;
// 	
// 	for(size_t i = 0; i < nInserters; ++i)
// 	{
// 		const InsertOperationTiming* pInsertOperationTiming = static_cast<const InsertOperationTiming*>(vInserters[i]);
// 		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pInsertOperationTiming, FIND_OPS);
// 		size_t nParallels = vParallels.size();
// 		
// 		bFindBeforeResult = bErBeforeResult = bErqBeforeResult = true;
// 		bFindAfterResult = bErAfterResult = bErqAfterResult = 
// 		bHasFindOpAfter = bHasErOpAfter = bHasErqOpAfter = false;
// 		
// 				
// 		for(size_t j = 0; j < nParallels; ++j)
// 		{
// 			// parallel find started rather than insert
// 			if (vParallels[j]->m_kOperationType & FIND) {
// 				const FindOperationTiming* pFindOperationTiming = static_cast<const FindOperationTiming*>(vParallels[j]);
// 				
// 				std::string s = pFindOperationTiming->m_tLockTime < vInserters[i]->m_tLockTime ? "<" : ">";
// 				std::cout 	<< "checkFind: " << COUT_PAIR(pFindOperationTiming->m_oResult) 
// 							<< ", ltr " << s << " ltw"
// 							<< std::endl;
// 				
// 				if (
// 					bFindBeforeResult &&
// 					pFindOperationTiming->m_tLockTime < pInsertOperationTiming->m_tLockTime &&
// 					pFindOperationTiming->m_oResult.second == pInsertOperationTiming->m_tValue
// 				) {
// 					bFindBeforeResult = false;
// 				}
// 				
// 				bHasFindOpAfter = pFindOperationTiming->m_tLockTime > pInsertOperationTiming->m_tLockTime;
// 				if (
// 					!bFindAfterResult &&
// 					bHasFindOpAfter &&
// 					pFindOperationTiming->m_oResult.second == pInsertOperationTiming->m_tValue
// 				) {
// 					bFindAfterResult = true;
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
// 				if (
// 					bErBeforeResult && 
// 					pEqualRangeOperationTiming->m_tLockTime < pInsertOperationTiming->m_tLockTime && 
// 					it != it_end
// 				) {
// 					bErBeforeResult = false;
// 				}
// 				
// 				bHasErOpAfter = pEqualRangeOperationTiming->m_tLockTime > pInsertOperationTiming->m_tLockTime;
// 				if (
// 					!bErAfterResult && 
// 					bHasErOpAfter && 
// 					it != it_end
// 				) {
// 					bErAfterResult = true;
// 				}
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
// 				
// 				if (
// 					bErqBeforeResult && 
// 					pEqualRangeQOperationTiming->m_tLockTime < pInsertOperationTiming->m_tLockTime && 
// 					it != it_end
// 				) {
// 					bErqBeforeResult = false;
// 				}
// 				
// 				bHasErqOpAfter = pEqualRangeQOperationTiming->m_tLockTime > pInsertOperationTiming->m_tLockTime;
// 				if (
// 					!bErqAfterResult && 
// 					bHasErqOpAfter && 
// 					it != it_end
// 				) {
// 					bErqAfterResult = true;
// 				}
// 			}
// 			
// 			bFindAfterResult 	= (bFindAfterResult	|| !bHasFindOpAfter);
// 			bErAfterResult 		= (bErAfterResult	|| !bHasErOpAfter);
// 			bErqAfterResult 	= (bErqAfterResult	|| !bHasErqOpAfter);
// 			
// 			bResult = bResult 	&& 
// 			bFindBeforeResult 	&& bFindAfterResult	&& 
// 			bErBeforeResult 	&& bErAfterResult 	&& 
// 			bErqBeforeResult 	&& bErqAfterResult;
// 		}
// 
// 		PRT_INS_FIND_BEFORE_RESULT_FAIL(bFindBeforeResult, pInsertOperationTiming, "find");
// 		PRT_INS_FIND_AFTER_RESULT_FAIL(bFindAfterResult, pInsertOperationTiming, "find");
// 
// 		PRT_INS_FIND_BEFORE_RESULT_FAIL(bErBeforeResult, pInsertOperationTiming, "equalRange");
// 		PRT_INS_FIND_AFTER_RESULT_FAIL(bErAfterResult, pInsertOperationTiming, "equalRange");
// 		
// 		PRT_INS_FIND_BEFORE_RESULT_FAIL(bErqBeforeResult, pInsertOperationTiming, "operator []");
// 		PRT_INS_FIND_AFTER_RESULT_FAIL(bErqAfterResult, pInsertOperationTiming, "operator []");
// 		
// 		vParallels.unlink();
// 	}
// 	vInserters.unlink();
// 	
// 	oHashMap.m_vTSOpTimings.clear();
// 	return bResult;
// }

bool checkInsertAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
{
	ThreadList lThreads;
	std::random_device oRandom;
	TSTHashMapInt::PairType oDefaultPair(12345678, 12345678);
	
	// run			
	runWriteAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, insertHashMapWorker);
	
	// check
	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
	
	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
	
	TSOperationTimingVector vInserters = oHashMap.m_vTSOpTimings.filterByType(INSERT);
	size_t nInserters = vInserters.size();
	TSOperationTimingVector vParallels;
	
	for(size_t i = 0; i < nInserters; ++i)
	{
		const InsertOperationTiming* pInsertOperationTiming = static_cast<const InsertOperationTiming*>(vInserters[i]);
		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pInsertOperationTiming, FIND_OPS);
		size_t nParallels = vParallels.size();
		
		CheckReadersFlags oFindCheckFlags, oErCheckFlags, oErqCheckFlags;
		
		for(size_t j = 0; j < nParallels; ++j)
		{
			// parallel find started rather than insert
			if (vParallels[j]->m_kOperationType & FIND) {
				bResult &= checkFindOpTimingForValue(vInserters, vParallels, i, j, pInsertOperationTiming->m_tValue, oFindCheckFlags);
				if (bResult) break;
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
				bResult &= checkErOpTimingForValue(vInserters, vParallels, i, j, pInsertOperationTiming->m_tValue, oErCheckFlags);
				if (bResult) break;
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
				bResult &= checkErOpTimingForValue(vInserters, vParallels, i, j, pInsertOperationTiming->m_tValue, oErqCheckFlags);
				if (bResult) break;
			}
		}

		PRT_INS_FIND_BEFORE_RESULT_FAIL(oFindCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult, pInsertOperationTiming, "find");
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oFindCheckFlags.m_kFlags & (CheckReadersFlags::kReadAfterResult | CheckReadersFlags::kWriterHasNoReadersAfter), 
			pInsertOperationTiming, 
			"find"
		);

		PRT_INS_FIND_BEFORE_RESULT_FAIL(oErCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult, pInsertOperationTiming, "equalRange");
		PRT_INS_FIND_AFTER_RESULT_FAIL(oErCheckFlags.m_kFlags & CheckReadersFlags::kReadAfterResult, pInsertOperationTiming, "equalRange");
		
		PRT_INS_FIND_BEFORE_RESULT_FAIL(oErqCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult, pInsertOperationTiming, "operator []");
		PRT_INS_FIND_AFTER_RESULT_FAIL(oErqCheckFlags.m_kFlags & CheckReadersFlags::kReadAfterResult, pInsertOperationTiming, "operator []");
		
		vParallels.unlink();
	}
	vInserters.unlink();
	
	oHashMap.m_vTSOpTimings.clear();
	return bResult;
}


#define PRT_RM_FIND_BEFORE_RESULT_FAIL(b_flag, p_pair, method)\
if (!b_flag)\
	cout 	<< " FAIL:     " \
			<< "Removed Pair: (" << p_pair->first << ", " << p_pair->second \
			<< ") not found before removing by `" << method << "`" << endl;
		
#define PRT_RM_FIND_AFTER_RESULT_FAIL(b_flag, p_pair, method)\
if (!b_flag)\
	cout 	<< " FAIL:     " \
			<< "Removed Pair: (" << p_pair->first << ", " << p_pair->second \
			<< ") found after removing by `" << method << "`" << endl;

bool checkRemoveAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
{
	ThreadList lThreads;
	std::random_device oRandom;
	TSTHashMapInt::PairType oDefaultPair(12345678, 12345678);
	
	// run			
	runWriteAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, removeHashMapWorker);
// 	
	// check
	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
	
	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
	
	TSOperationTimingVector vRemovers = oHashMap.m_vTSOpTimings.filterByType(INSERT);
	size_t nRemovers = vRemovers.size();
	TSOperationTimingVector vParallels;
	
	for(size_t i = 0; i < nRemovers; ++i)
	{
		const RemoveOperationTiming* pRemoveOperationTiming = static_cast<const RemoveOperationTiming*>(vRemovers[i]);
		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pRemoveOperationTiming, FIND_OPS);
		size_t nParallels = vParallels.size();
				
		EqualRangeType::const_iterator 
			it = pRemoveOperationTiming->m_lResult.begin(), 
			it_end = pRemoveOperationTiming->m_lResult.end();
		
		for(; it != it_end; ++it) 
		{
			CheckReadersFlags oFindCheckFlags, oErCheckFlags, oErqCheckFlags;
			ConstSharedPtrPairType pPair;
			
			for(size_t j = 0; j < nParallels; ++j)
			{
			// parallel find started rather than insert
				if (vParallels[j]->m_kOperationType & FIND) {
					CheckReadersFlags oFindCheckFlags;
					
					pPair = it->lock();
					bResult &= checkFindOpTimingForValue(vRemovers, vParallels, i, j, pPair->second, oFindCheckFlags);
					
					PRT_RM_FIND_BEFORE_RESULT_FAIL(
						oFindCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult, pPair, "find"
					);
					PRT_RM_FIND_AFTER_RESULT_FAIL(
						!(oFindCheckFlags.m_kFlags & CheckReadersFlags::kReadAfterResult), pPair, "find"
					);
				} /*else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
					CheckReadersFlags oErCheckFlags;
					
					pPair = it->lock();
					bResult &= checkErOpTimingForValue(vRemovers, vParallels, i, j, pPair->second, oErCheckFlags);
					
					PRT_RM_FIND_BEFORE_RESULT_FAIL(
						!(oErCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult), pPair, "equalRange"
					);
					PRT_RM_FIND_AFTER_RESULT_FAIL(
						!(oErCheckFlags.m_kFlags & CheckReadersFlags::kReadAfterResult), pPair, "equalRange"
					);
				} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
					CheckReadersFlags oErqCheckFlags;
					
					pPair = it->lock();
					bResult &= checkErOpTimingForValue(vRemovers, vParallels, i, j, pPair->second, oErqCheckFlags);
					
					PRT_RM_FIND_BEFORE_RESULT_FAIL(
						oErqCheckFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult, pPair, "operator []"
					);
					PRT_RM_FIND_AFTER_RESULT_FAIL(
						!(oErqCheckFlags.m_kFlags & CheckReadersFlags::kReadAfterResult), pPair, "operator []"
					);
				}*/
			}
		}

		vParallels.unlink();
	}
	vRemovers.unlink();
	
	oHashMap.m_vTSOpTimings.clear();
	return bResult;
}

#define PRT_RESULT(b_result, message)\
{\
	string sStatus = b_result ? " SUCCESS:   " : " FAIL:      ";\
	cout << sStatus << message << endl;\
}

#define PRT_NOTE(message)\
	cout << "NOTE:      " << message << endl;
	
#define PRT_SIZE_CHECK_FAIL(b_result, size, expected_size)\
if (!b_result)\
	cout << " FAIL:      " << "hashmap size is: " << size << ", expexted: " << expected_size << endl

int main()
{
	TSTHashMapInt 	oHashMap;
	
	int			iStatus = 0;
	bool		bResult, bSizeCheck;
	size_t		nThreadCount;
	
	
	PRT_NOTE("Checking find operations about inserts");
	bResult = checkInsertAndFindThreads(oHashMap, 0, 9, 10);
	bSizeCheck = oHashMap.size() == 100;
	PRT_SIZE_CHECK_FAIL(bSizeCheck, oHashMap.size(), 100);
	iStatus = !bResult || !bSizeCheck;
	PRT_RESULT(bResult, "Checking find operations about inserts");
	
	
	PRT_NOTE("Checking find operations about removes");
	bResult = checkRemoveAndFindThreads(oHashMap, 0, 9, 10);
	bSizeCheck = oHashMap.size() == 0;
	PRT_SIZE_CHECK_FAIL(bSizeCheck, oHashMap.size(), 0);
	iStatus = !bResult || !bSizeCheck;
	PRT_RESULT(bResult, "Checking find operations about removes");
	
	return iStatus;
}
