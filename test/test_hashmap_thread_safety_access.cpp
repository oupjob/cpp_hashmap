#include "hashmap.h"
#include <iostream>
#include <thread>
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
typedef TSTHashMapInt::WeakPtrPairType					WeakPtrPairType;

typedef std::lock_guard<std::mutex> 	LockGuard;
typedef std::list<std::thread>			ThreadList;

#define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"

#define PRT_INS_FIND_BEFORE_RESULT_FAIL(b_flag, ins_timing, method, b_result)\
if (!b_flag) {\
	cout 	<< " FAIL:     " \
			<< "Inserted Pair: (" << ins_timing->m_tKey << ", " << ins_timing->m_tValue \
			<< ") not found by `" << method << "`" << " before insertion" << endl;\
	b_result = false;\
}
		
#define PRT_INS_FIND_AFTER_RESULT_FAIL(b_flag, ins_timing, method, b_result)\
if (!b_flag) {\
	cout 	<< " FAIL:     " \
			<< "Inserted Pair: (" << ins_timing->m_tKey << ", " << ins_timing->m_tValue \
			<< ") not found by `" << method << "`" << " after insertion" << endl;\
	b_result = false;\
}

bool checkInsertAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
{	
	// run			
	runWorkerAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, insertHashMapWorker);
	
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
		
		CheckFindersFlags oFindCheckFlags, oErCheckFlags, oErqCheckFlags;
		PairType oExpectedPair(pInsertOperationTiming->m_tKey, pInsertOperationTiming->m_tValue);
		
		for(size_t j = 0; j < nParallels; ++j)
		{
			// parallel find started rather than insert
			if (vParallels[j]->m_kOperationType & FIND) {
				if (static_cast<const FindOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkFindOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oFindCheckFlags);
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
				if (static_cast<const EqualRangeOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkErOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oErCheckFlags);
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
				if (static_cast<const EqualRangeQOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkErOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oErqCheckFlags);
			}
		}
		
		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oFindCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore),
			pInsertOperationTiming, 
			"find",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter), 
			pInsertOperationTiming, 
			"find",
			bResult
		);

		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oErCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore), 
			pInsertOperationTiming, 
			"equalRange",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oErCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter),
			pInsertOperationTiming, 
			"equalRange",
			bResult
		);
		
		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oErqCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore), 
			pInsertOperationTiming, 
			"operator []",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oErqCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter),
			pInsertOperationTiming, 
			"operator []",
			bResult
		);
		
		vParallels.unlink();
	}
	vInserters.unlink();
	
	oHashMap.m_vTSOpTimings.clear();
	return bResult;
}


#define PRT_RM_FIND_BEFORE_RESULT_FAIL(b_flag, p_pair, method, b_result)\
if (!b_flag) {\
	cout 	<< " FAIL:     " \
			<< "Removed Pair: (" << p_pair->first << ", " << p_pair->second \
			<< ") not found before removing by `" << method << "`" << endl;\
	b_result = false;\
}
		
#define PRT_RM_FIND_AFTER_RESULT_FAIL(b_flag, p_pair, method, b_result)\
if (!b_flag) {\
	cout 	<< " FAIL:     " \
			<< "Removed Pair: (" << p_pair->first << ", " << p_pair->second \
			<< ") found after removing by `" << method << "`" << endl;\
	b_result = false;\
}

// This function does not check searches before deletion, since deletion behaves randomly during multi-threaded operation.
// Instead of the expected element after deletion, its neighbor can be obtained.
// If you want to verify this, then uncomment the code in the body of this function and run this test several times.

bool checkRemoveAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
{	
	// run			
	runWorkerAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, removeHashMapWorker);
	
	// check
	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
	
// 	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
// 	
// 	TSOperationTimingVector vRemovers = oHashMap.m_vTSOpTimings.filterByType(REMOVE);
// 	size_t nRemovers = vRemovers.size();
// 	TSOperationTimingVector vParallels;
// 	
// 	for(size_t i = 0; i < nRemovers; ++i)
// 	{
// 		const RemoveOperationTiming* pRemoveOperationTiming = static_cast<const RemoveOperationTiming*>(vRemovers[i]);
// 		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pRemoveOperationTiming, FIND_OPS);
// 		size_t nParallels = vParallels.size();
// 				
// 		RemovedRangeType::const_iterator 
// 			it = pRemoveOperationTiming->m_lResult.begin(), 
// 			it_end = pRemoveOperationTiming->m_lResult.end();
// 		
// 		for(; it != it_end; ++it) 
// 		{
// 			CheckFindersFlags oFindCheckFlags, oErCheckFlags, oErqCheckFlags;
// 			ConstSharedPtrPairType pPair = *it;;
// 			
// 			for(size_t j = 0; j < nParallels; ++j)
// 			{
// 				// parallel find started rather than insert
// 				if (vParallels[j]->m_kOperationType & FIND) {
// 					if (static_cast<const FindOperationTiming*>(vParallels[j])->m_tKey != pPair->first)
// 						continue;
// 					
// 					checkFindOpTimingForPair(vRemovers, vParallels, i, j, *pPair, oFindCheckFlags);
// 				} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
// 					if (static_cast<const EqualRangeOperationTiming*>(vParallels[j])->m_tKey != pPair->first)
// 						continue;
// 					
// 					checkErOpTimingForPair(vRemovers, vParallels, i, j, *pPair, oErCheckFlags);
// 				} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
// 					if (static_cast<const EqualRangeQOperationTiming*>(vParallels[j])->m_tKey != pPair->first)
// 						continue;
// 					
// 					checkErOpTimingForPair(vRemovers, vParallels, i, j, *pPair, oErqCheckFlags);
// 				}
// 			}
// 								
// 			PRT_RM_FIND_BEFORE_RESULT_FAIL(
// 				oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundBefore | CheckFindersFlags::kWriterHasNoFindersBefore), 
// 				pPair, 
// 				"find", 
// 				bResult
// 			);
// 			PRT_RM_FIND_AFTER_RESULT_FAIL(
// 				!(oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter)),
// 				pPair, 
// 				"find",
// 				bResult
// 			);
// 			
// 			PRT_RM_FIND_BEFORE_RESULT_FAIL(
// 				oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundBefore | CheckFindersFlags::kWriterHasNoFindersBefore), 
// 				pPair, 
// 				"equalRange", 
// 				bResult
// // 			);
// 			PRT_RM_FIND_AFTER_RESULT_FAIL(
// 				!(oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter)),
// 				pPair, 
// 				"equalRange",
// 				bResult
// 			);
// 			
// 			PRT_RM_FIND_BEFORE_RESULT_FAIL(
// 				oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundBefore | CheckFindersFlags::kWriterHasNoFindersBefore), 
// 				pPair, 
// 				"operator []", 
// 				bResult
// // 		);
// 			PRT_RM_FIND_AFTER_RESULT_FAIL(
// 				!(oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter)),
// 				pPair, 
// 				"operator []",
// 				bResult
// 			);
// 		}
// 
// 		vParallels.unlink();
// 	}
// 	vRemovers.unlink();
//	
//	oHashMap.m_vTSOpTimings.clear();
	return bResult;
}

bool checkExtendAndFindThreads(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount) 
{
	runWorkerAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, extendHashMapWorker, true);
	
	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
	
	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
	
	TSOperationTimingVector vExtenders = oHashMap.m_vTSOpTimings.filterByType(EXTEND);
	size_t nExtenders = vExtenders.size();
	TSOperationTimingVector vParallels;
	
	for(size_t i = 0; i < nExtenders; ++i)
	{
		const ExtendOperationTiming* pExtendOperationTiming = static_cast<const ExtendOperationTiming*>(vExtenders[i]);
		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pExtendOperationTiming, FIND_OPS);
		size_t nParallels = vParallels.size();
				
		for(size_t j = 0; j < nParallels; ++j)
		{
			// parallel find started rather than insert
			if (vParallels[j]->m_kOperationType & FIND) {
				const FindOperationTiming* pFindOperationTiming = static_cast<const FindOperationTiming*>(vParallels[j]);
				
				cout << COUT_PAIR(pFindOperationTiming->m_oResult) << " key = " << pFindOperationTiming->m_tKey << endl;
				if (pFindOperationTiming->m_oResult.first != pFindOperationTiming->m_tKey) {
					cout << " FAIL:     " << "Pairs for key=" << pFindOperationTiming->m_tKey << " is not found by `find`" << endl;
					bResult = false;
				}
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
				const EqualRangeOperationTiming* pEqualRangeOperationTiming = static_cast<const EqualRangeOperationTiming*>(vParallels[j]);
				
				EqualRangeType::const_iterator 
					it = pEqualRangeOperationTiming->m_lResult.begin(),
					it_end = pEqualRangeOperationTiming->m_lResult.end();
					
				if (it == it_end)
					cout << " FAIL:     " << "Nothing found by `equalRange`" << endl;
				
				for(; it != it_end; ++it)
				{
					if (SharedPtrPairType pPair = it->lock()) {
						if (pPair->first != pEqualRangeOperationTiming->m_tKey)
							cout 
								<< " FAIL:     " 
								<< "Pairs for key=" << pEqualRangeOperationTiming->m_tKey 
								<< " is not found by `equalRange`" 
							<< endl;
					} else {
						cout 
							<< " FAIL:     " 
							<< "Cannot lock weak_ptr for key " << pEqualRangeOperationTiming->m_tKey 
							<< " in `equalRange` check"
						<< endl;
					}
				}
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
				const EqualRangeQOperationTiming* 
					pEqualRangeQOperationTiming = static_cast<const EqualRangeQOperationTiming*>(vParallels[j]);
				
				EqualRangeType::const_iterator 
					it = pEqualRangeQOperationTiming->m_lResult.begin(),
					it_end = pEqualRangeQOperationTiming->m_lResult.end();
					
				if (it == it_end)
					cout << " FAIL:     " << "Nothing found by `operator []`" << endl; 
				
				for(; it != it_end; ++it)
				{
					if (SharedPtrPairType pPair = it->lock()) {
						if (pPair->first != pEqualRangeQOperationTiming->m_tKey)
							cout 
								<< " FAIL:     " 
								<< "Pairs for key=" << pEqualRangeQOperationTiming->m_tKey 
								<< " is not found by `operator []`" 
							<< endl;
					} else {
						cout 
							<< " FAIL:     " 
							<< "Cannot lock weak_ptr for key " << pEqualRangeQOperationTiming->m_tKey 
							<< " in `operator []` check"
						<< endl;
					}
				}
			}
		}

		vParallels.unlink();
	}
	vExtenders.unlink();
	
	oHashMap.m_vTSOpTimings.clear();
	return bResult;
}

template<typename WorkerFunction>
bool simpleCheck(TSTHashMapInt& oHashMap, int iStartKey, int iEndKey, size_t nCount, WorkerFunction threadWorker, bool bInsertBefore = false)
{
	// run 
	runWorkerAndFindThreads(oHashMap, iStartKey, iEndKey, nCount, threadWorker, bInsertBefore);
	
	bool bResult = checkLockTimes(oHashMap.m_vTSOpTimings);
	
	if (!bInsertBefore)
		return bResult;
	
	oHashMap.m_vTSOpTimings.sortByEndTimeAsc();
	
	TSOperationTimingVector vInserters = oHashMap.m_vTSOpTimings.filterByType(INSERT);
	size_t nInserters = vInserters.size();
	TSOperationTimingVector vParallels;
	
	for(size_t i = 0; i < nInserters; ++i)
	{
		const InsertOperationTiming* pInsertOperationTiming = static_cast<const InsertOperationTiming*>(vInserters[i]);
		vParallels = oHashMap.m_vTSOpTimings.filterParallelWorkersByType(pInsertOperationTiming, FIND_OPS);
		size_t nParallels = vParallels.size();
		
		CheckFindersFlags oFindCheckFlags, oErCheckFlags, oErqCheckFlags;
		PairType oExpectedPair(pInsertOperationTiming->m_tKey, pInsertOperationTiming->m_tValue);
		
		for(size_t j = 0; j < nParallels; ++j)
		{
			// parallel find started rather than insert
			if (vParallels[j]->m_kOperationType & FIND) {
				if (static_cast<const FindOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkFindOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oFindCheckFlags);
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE) {
				if (static_cast<const EqualRangeOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkErOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oErCheckFlags);
			} else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) {
				if (static_cast<const EqualRangeQOperationTiming*>(vParallels[j])->m_tKey != oExpectedPair.first)
					continue;
				
				checkErOpTimingForPair(vInserters, vParallels, i, j, oExpectedPair, oErqCheckFlags);
			}
		}
		
		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oFindCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore),
			pInsertOperationTiming, 
			"find",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oFindCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter), 
			pInsertOperationTiming, 
			"find",
			bResult
		);

		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oErCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore), 
			pInsertOperationTiming, 
			"equalRange",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oErCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter),
			pInsertOperationTiming, 
			"equalRange",
			bResult
		);
		
		PRT_INS_FIND_BEFORE_RESULT_FAIL(
			!(oErqCheckFlags.m_kFlags & CheckFindersFlags::kFoundBefore), 
			pInsertOperationTiming, 
			"operator []",
			bResult
		);
		PRT_INS_FIND_AFTER_RESULT_FAIL(
			oErqCheckFlags.m_kFlags & (CheckFindersFlags::kFoundAfter | CheckFindersFlags::kWriterHasNoFindersAfter),
			pInsertOperationTiming, 
			"operator []",
			bResult
		);
		
		vParallels.unlink();
	}
	vInserters.unlink();
		
	
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
	cout << " FAIL:      " << "hashmap size is: " << size << ", expected: " << expected_size << endl

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
	iStatus = iStatus && (!bResult || !bSizeCheck);
	PRT_RESULT(bResult, "Checking find operations about inserts");
	
	
	PRT_NOTE("Checking find operations about removes");
	bResult = checkRemoveAndFindThreads(oHashMap, 0, 9, 10);
	bSizeCheck = oHashMap.size() == 0;
	PRT_SIZE_CHECK_FAIL(bSizeCheck, oHashMap.size(), 0);
	iStatus = iStatus && (!bResult || !bSizeCheck);
	PRT_RESULT(bResult, "Checking find operations about removes");
	oHashMap.clear();
	
	
	PRT_NOTE("Checking find operations about extends");
	bResult = checkExtendAndFindThreads(oHashMap, 0, 9, 10);
	cout << "size(H) = " << oHashMap.size() << endl;
	iStatus = iStatus && !bResult;;
	PRT_RESULT(bResult, "Checking find operations about extends");
	oHashMap.clear();
	
	
	PRT_NOTE("Checking find operations about clears");
	bResult = simpleCheck(oHashMap, 0, 9, 10, clearHashMapWorker, true);
	bSizeCheck = oHashMap.size() == 0;
	PRT_SIZE_CHECK_FAIL(bSizeCheck, oHashMap.size(), 0);
	iStatus = iStatus && (!bResult || !bSizeCheck);
	PRT_RESULT(bResult, "Checking find operations about clears");
	
	
	PRT_NOTE("Checking find operations about empty operations");
	bResult = simpleCheck(oHashMap, 0, 9, 10, emptyHashMapWorker, false);
	iStatus = iStatus && !bResult;
	PRT_RESULT(bResult, "Checking find operations about empty operations");
	
	
	PRT_NOTE("Checking find operations about size operations");
	bResult = simpleCheck(oHashMap, 0, 9, 10, sizeHashMapWorker, false);
	iStatus = iStatus && !bResult;
	PRT_RESULT(bResult, "Checking find operations about size operations");
	
	
	PRT_NOTE("Checking find operations about capacity operations");
	bResult = simpleCheck(oHashMap, 0, 9, 10, capacityHashMapWorker, false);
	iStatus = iStatus && !bResult;
	PRT_RESULT(bResult, "Checking find operations about capacity operations");
	
	return iStatus;
}
