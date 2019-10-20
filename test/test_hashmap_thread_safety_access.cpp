// #include "thread_safety_testable_hashmap.h"
// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <chrono>
// #include <ctime>
// #include <iomanip>
// #include <exception>
// #include <random>
// 
// using namespace std;
// 
// typedef ThreadSafetyTestableHashMap<> 	HashMapInt;
// typedef HashMapInt::PairType 			PairType;
// typedef HashMapInt::ConstRefPairType	ConstRefPairType;
// 
// typedef std::lock_guard<std::mutex> 	LockGuard;
// typedef std::list<std::thread>			ThreadList;
// 
// #define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"
// 
// enum ChkOperationType 
// {
// 	CHK_FIND_EXISTS,
// 	CHK_FIND_NONEXISTS
// };
// 
// void checkFind(
// 	ChkOperationType 		kCheckType, 
// 	const HashMapInt& 		oHashMap, 
// 	int 					iKey, 
// 	size_t 					iPos, 
// 	size_t 					nExpectedCount, 
// 	std::mutex& 			mMutex
// ) {
// 	PairType oDefaultPair = PairType(12345678, 12345678), oExpectedPair;
// 	bool bResult = true;
// 	size_t nCount = 0;
// 	
// 	for(size_t i = iPos; i <= nExpectedCount; ++i)
// 	{
// 		ConstRefPairType oPair = oHashMap.find(iKey, oDefaultPair, i);
// 		
// 		if (kCheckType == CHK_FIND_EXISTS) {
// 			bResult = (oPair == oExpectedPair);
// 			if (!bResult) {
// 				LockGuard lLock(mMutex);
// 				cout << " FAIL:    " << "Expected pair: " << COUT_PAIR(oExpectedPair) << " not found" << endl;
// 			} else {
// 				++nCount;
// 			}
// 		} else if (kCheckType == CHK_FIND_NONEXISTS) {
// 			bResult = (oPair == oDefaultPair);
// 			if (!bResult) {
// 				LockGuard lLock(mMutex);
// 				cout << " FAIL:    " << "Pair: " << COUT_PAIR(oExpectedPair) << " found, but not expected" << endl; 
// 			} else {
// 				++nCount;
// 			}
// 		}
// 	}
// 	
// 	if (nCount != nExpectedCount) {
// 		if (kCheckType == CHK_FIND_EXISTS) {
// 			LockGuard lLock(mMutex);
// 			cout << " FAIL:    " << "Found " << nCount << " inserted pairs, expected: " << nExpectedCount << endl;
// 		} else if (kCheckType == CHK_FIND_NONEXISTS) {
// 			LockGuard lLock(mMutex);
// 			cout << " FAIL:    " << "Found " << nCount << " noninserted pairs, expected: " << nExpectedCount << endl;
// 		}
// 		
// 		bResult = false;
// 	}
// }
// 
// void runFindThread(
// 	HashMapInt& oHashMap, int iKey, size_t iPos, size_t nCount
// ) {
// 	ThreadList lThreads;
// 	HashMapInt::PairType oDefaultPair(12345678, 12345678);
// 	for(; iPos < nCount; ++iPos)
// 	{
//  		lThreads.push_back(std::thread([&]{ oHashMap.find(iKey, oDefaultPair, iPos); }));
// 	}
// 	
// 	ThreadList::iterator it = lThreads.begin(), it_end = lThreads.end();
// 	for(; it != it_end; ++it) 
// 		(*it).join();
// }
// 
// void checkInsertAndFindThreads(
// 	HashMapInt& oHashMap, int iKey, size_t iPos, size_t nCount
// ) {
// 	ThreadList lThreads;
// 	std::random_device oRandom;
// 	HashMapInt::PairType oDefaultPair(12345678, 12345678);
// 	
// 	auto find = [&]{ oHashMap.find(iKey, oDefaultPair); };
// 	auto eq_range = [&] { oHashMap.equalRange(iKey); };
// 	auto eq_range_q = [&] { oHashMap[iKey]; };
// 	auto insert = [&]{ oHashMap.insert(iKey, iKey * 10 + iPos); };
// 	
// 	for(; iPos < nCount; ++iPos)
// 	{
// 		if (oRandom() % 2) {
// 			lThreads.push_back(std::thread(insert));
// 		} else {
// 			lThreads.push_back(std::thread(find));
// 			lThreads.push_back(std::thread(eq_range));
// 			lThreads.push_back(std::thread(eq_range_q));
// 		}
// 	}
// 	
// 	ThreadList::iterator it = lThreads.begin(), it_end = lThreads.end();
// 	for(; it != it_end; ++it) 
// 		(*it).join();
// }
// 
// typedef ostream& (*DumpFunc)(ostream&, const HashMapInt::OperationTiming&);
// typedef DumpFunc DumpFuncMap[10];
// 
// void dumpOpTimingVector(
// 	const HashMapInt::OperationTimingVector& 	vOpTimings, 
// 	const DumpFuncMap& 							aDumpFuncMap,
// 	size_t 										iBegin=0, 
// 	size_t 										iEnd=0
// ) {
// 	if (iEnd == 0)
// 		iEnd = vOpTimings.size() - iBegin - 1;
// 	
// 	for(size_t i = iBegin; i != iEnd; ++i)
// 	{
// 		const HashMapInt::OperationTiming& oOpTiming = vOpTimings[i];
// 		aDumpFuncMap[oOpTiming.m_kOperationType](cout, oOpTiming);
// 	}
// }
// 
// // ostream& dumpOpTimingHeader(ostream& stream, const HashMapInt::OperationTiming& oOpTiming)
// // {
// // 	stream 	<< " StartTime: " << put_time(oOpTiming.m_tStartTime)
// // 			<< ", LockTime: " << put_time(oOpTiming.m_tLockTime)
// // 			<< ", EndTime: " << put_time(oOpTiming.m_tEndTime)
// // 			
// // 	return stream;
// // }
// 
// HashMapInt::OperationTiming::DurationType::rep opTimePointToInteger(const HashMapInt::OperationTiming::TimeType& tTime)
// {
// 	return chrono::duration_cast<HashMapInt::OperationTiming::DurationType>(tTime.time_since_epoch()).count();
// }
// 
// ostream&  operator << (ostream& stream, const HashMapInt::OperationTiming& oOpTiming)
// {
// 	stream 	<< " StartTime: " << opTimePointToInteger(oOpTiming.m_tStartTime)
// 			<< ", LockTime: " << opTimePointToInteger(oOpTiming.m_tLockTime)
// 			<< ", EndTime: " << opTimePointToInteger(oOpTiming.m_tEndTime);
// 			
// 	return stream;
// }
// 
// ostream& dumpFindOpTiming(ostream& stream, const HashMapInt::OperationTiming& oOpTiming) 
// {
// 	stream 	<< "OpTiming Type: 'FIND', " << oOpTiming
// 			<< ", Key=" << oOpTiming.m_uOperationDesc.m_oFindOperationDesc.m_tKey 
// 			<< ", Pos=" <<  oOpTiming.m_uOperationDesc.m_oFindOperationDesc.m_iPos
// 			<< ", Result=" << COUT_PAIR(oOpTiming.m_uOperationDesc.m_oFindOperationDesc.m_oResult)
// 			<< endl;
// 
// 	return stream;
// }
// 
// ostream& dumpEqualRangeOpTiming(ostream& stream, const HashMapInt::OperationTiming& oOpTiming) 
// {
// 	stream 	<< "OpTiming Type: 'EQUAL_RANGE', " << oOpTiming
// 			<< ", Key=" << oOpTiming.m_uOperationDesc.m_oEqualRangeOperationDesc.m_tKey 
// 			<< ", Pos=" <<  oOpTiming.m_uOperationDesc.m_oEqualRangeOperationDesc.m_iPos
// 			<< ", Count=" << oOpTiming.m_uOperationDesc.m_oEqualRangeOperationDesc.m_nCount
// 			<< ", Len(Result)=" << oOpTiming.m_uOperationDesc.m_oEqualRangeOperationDesc.m_lResult.size()
// 			<< endl;
// 
// 	return stream;
// }
// 
// ostream& dumpEqualRangeQOpTiming(ostream& stream, const HashMapInt::OperationTiming& oOpTiming) 
// {
// 	stream 	<< "OpTiming Type: 'OPERATOR []', " << oOpTiming
// 			<< ", Key=" << oOpTiming.m_uOperationDesc.m_oEqualRangeQOperationDesc.m_tKey 
// 			<< ", Len(Result)=" << oOpTiming.m_uOperationDesc.m_oEqualRangeQOperationDesc.m_lResult.size()
// 			<< endl;
// 
// 	return stream;
// }
// 
// ostream& dumpInsertOpTiming(ostream& stream, const HashMapInt::OperationTiming& oOpTiming) 
// {
// 	stream 	<< "OpTiming Type: 'INSERT', " << oOpTiming 
// 			<< ", Key=" << oOpTiming.m_uOperationDesc.m_oInsertOperationDesc.m_tKey 
// 			<< ", Value=" << oOpTiming.m_uOperationDesc.m_oInsertOperationDesc.m_tValue
// 			<< endl;
// 
// 	return stream;
// }
// 
// int main() 
// {	
// 	DumpFuncMap aDumpFuncMap = {
// 		dumpFindOpTiming, 			// FIND
// 		dumpEqualRangeOpTiming,		// EQUAL_RANGE 
// 		dumpEqualRangeQOpTiming,	// EQUAL_RANGE_Q []
// 		dumpInsertOpTiming			// INSERT
// 	};
// 	
// 	HashMapInt 			oHashMap;
// 	std::mutex			mMutex;
// 	
// 	int 		iStatus = 0;
// 	bool 		bResult;
// 	size_t		iLastOpDescIndex, iCurOpDescIndex;
// 	
// 	HashMapInt::OperationTiming::TimeType t = HashMapInt::OperationTiming::ClockType::now();
// 	
// 	checkInsertAndFindThreads(oHashMap, 0, 0, 10);
// 	
//  	dumpOpTimingVector(oHashMap.m_vOperationTimings, aDumpFuncMap);
// 	
// 	return iStatus;
// }

int main()
{
	return 0;
}
