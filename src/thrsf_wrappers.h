// #ifndef THRSF_WRAPPERS_H_
// #define THRSF_WRAPPERS_H_
// 
// #include <iostream>
// #include <mutex>
// #include <chrono>
// #include <ctime>
// #include <iomanip>
// #include <random>
// #include <algorithm>
// 
// #include "hashmap.h"
// 
// //  +===========================================================================================
// // DATA STRUCTURES +============================================================================
// //  +===========================================================================================
// 
// enum OperationType {
// 	UNKNOWN			= 0,
// 	
// 	FIND 			= 1,
// 	EQUAL_RANGE		= FIND << 1,
// 	EQUAL_RANGE_Q	= EQUAL_RANGE << 1,
// 	SIZE			= EQUAL_RANGE_Q << 1,
// 	CAPACITY		= SIZE << 1,
// 	EMPTY			= CAPACITY << 1,
// 	
// 	INSERT			= EMPTY << 1,
// 	REMOVE			= INSERT << 1,
// 	EXTEND			= REMOVE << 1,
// 	CLEAR			= EXTEND << 1,
// 	
// 	FIND_OPS 		= FIND | EQUAL_RANGE | EQUAL_RANGE_Q,
// 	KEYED_OPS 		= FIND_OPS | INSERT | REMOVE,
// 	POSITIONAL_OPS	= FIND | EQUAL_RANGE | REMOVE,
// 	RANGE_RES_OPS	= EQUAL_RANGE | EQUAL_RANGE_Q | REMOVE,
// 	COUNTED_OPS		= EQUAL_RANGE | REMOVE,
// 	
// 	READ_OPS		= FIND_OPS | SIZE | CAPACITY | EMPTY,
// 	WRITE_OPS		= INSERT | REMOVE | EXTEND | CLEAR
// };
// 
// //  +===========================================================================================
// // RESULTS STRUCTURES +=========================================================================
// //  +===========================================================================================
// 
// template <
// 	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<int>::HashFuncArgType, size_t) = stdHashFunction
// >
// class ThreadSafetyTestableHashMap;
// 
// typedef HashMap<int, int>					HashMapInt;
// typedef HashMap<int, int>::PairType			PairType;
// typedef HashMap<int, int>::EqualRangeType	EqualRangeType;
// typedef std::list<PairType>					PairEqRangeType;
// 
// struct OperationTimingInt 
// {
// 	typedef std::chrono::high_resolution_clock 					ClockType;
// 	typedef std::chrono::nanoseconds 							DurationType;
// 	typedef std::chrono::time_point<ClockType>					TimeType;
// 	
// 	OperationType			m_kOperationType;
// 	
// 	TimeType				m_tStartTime;
// 	TimeType				m_tLockTime;
// 	TimeType				m_tEndTime;
// 	
// 	OperationTimingInt(OperationType kOperationType) : m_kOperationType(kOperationType) {}
// 	virtual ~OperationTimingInt() {}
// 	
// 	virtual std::ostream& dump(std::ostream& stream) const;
// 	virtual std::ostream& operator << (std::ostream& stream) const;
// };
// 
// struct IFindOpsTiming : OperationTimingInt
// {
// 	int					m_tKey;
// 	
// 	IFindOpsTiming(OperationType kOperationType) : OperationTimingInt(kOperationType) {}
// 	virtual ~IFindOpsTiming() {}
// };
// 
// struct FindOperationTiming : IFindOpsTiming {
// 	size_t				m_iPos;
// 	size_t				m_nCount;
// 	PairType			m_oResult;
// 	
// 	FindOperationTiming() : IFindOpsTiming(FIND) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct EqualRangeOperationTiming : IFindOpsTiming {
// 	size_t				m_iPos;
// 	size_t				m_nCount;
// 	EqualRangeType		m_lResult;
// 	
// 	EqualRangeOperationTiming() : IFindOpsTiming(EQUAL_RANGE) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct EqualRangeQOperationTiming : IFindOpsTiming {
// 	EqualRangeType		m_lResult;
// 	
// 	EqualRangeQOperationTiming() : IFindOpsTiming(EQUAL_RANGE_Q) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct InsertOperationTiming : OperationTimingInt {
// 	int					m_tKey;
// 	int					m_tValue;
// 	
// 	InsertOperationTiming() : OperationTimingInt(INSERT) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct RemoveOperationTiming : OperationTimingInt {
// 	int					m_tKey;
// 	size_t				m_iPos;
// 	size_t				m_nCount;
// 	EqualRangeType		m_lResult;
// 	
// 	RemoveOperationTiming() : OperationTimingInt(REMOVE) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// // 
// struct ExtendOperationTiming : OperationTimingInt {
// 	size_t 				m_nNewCapacity;
// 	
// 	ExtendOperationTiming() : OperationTimingInt(EXTEND) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct SizeOperationTiming : OperationTimingInt {
// 	size_t 				m_nTSOpTimings;
// 	
// 	SizeOperationTiming() : OperationTimingInt(SIZE) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct CapacityOperationTiming : OperationTimingInt {
// 	size_t 				m_nCapacity;
// 	
// 	CapacityOperationTiming() : OperationTimingInt(CAPACITY) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct EmptyOperationTiming : OperationTimingInt {
// 	bool 				m_bIsEmpty;
// 	
// 	EmptyOperationTiming() : OperationTimingInt(EMPTY) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// struct ClearOperationTiming : OperationTimingInt {
// 	size_t				m_nNewCapacity;
// 	size_t				m_nNewSize;
// 	
// 	ClearOperationTiming() : OperationTimingInt(CLEAR) {}
// 	
// 	std::ostream& dump(std::ostream& stream) const;
// };
// 
// //  +===========================================================================================
// // RESULTS STRUCTURES RANGE INTERFACE +=========================================================
// //  +=========================================================================================== 
// 
// struct TSOperationTimingVector {
// 	std::vector<OperationTimingInt*>	m_vOperationTimings;
// 	std::mutex							m_mMutex;
// 
// 	TSOperationTimingVector() {}
// 	TSOperationTimingVector(const TSOperationTimingVector& vSrc);
// 	TSOperationTimingVector(TSOperationTimingVector&& vSrc);
// 	~TSOperationTimingVector() { clear(); }
// 	
// 	void dump(size_t iBegin=0, size_t iEnd=0);
// 	void clear();
// 	
// 	TSOperationTimingVector filterByType(OperationType kOperationType) const;
// 	TSOperationTimingVector filterParallelWorkers(const OperationTimingInt* oOpTiming) const;
// 	TSOperationTimingVector filterParallelWorkersByType(const OperationTimingInt* oOpTiming, OperationType kOperationType) const;
// 	
// 	TSOperationTimingVector& operator = (const TSOperationTimingVector& vSrc);
// 	TSOperationTimingVector& operator = (TSOperationTimingVector&& vSrc);
// 	
// 	const OperationTimingInt* operator [] (size_t i) { return m_vOperationTimings[i]; } 
// 	size_t size() const { return m_vOperationTimings.size(); }
// 	
// 	void sortByEndTimeAscAndCalcLockTimes();
// };
// 
// 
// TSOperationTimingVector::TSOperationTimingVector(const TSOperationTimingVector& vSrc)
// {
// 	m_vOperationTimings = vSrc.m_vOperationTimings;
// }
// 
// TSOperationTimingVector::TSOperationTimingVector(TSOperationTimingVector&& vSrc)
// {
// 	std::cout << "TSOperationTimingVector(TSOperationTimingVector&& vSrc)" << std::endl;
// 	m_vOperationTimings = std::move(vSrc.m_vOperationTimings);
// 	vSrc.m_vOperationTimings.clear();
// }
// 
// TSOperationTimingVector& TSOperationTimingVector::operator = (const TSOperationTimingVector& vSrc)
// {
// 	m_vOperationTimings = vSrc.m_vOperationTimings;
// }
// 
// TSOperationTimingVector& TSOperationTimingVector::operator = (TSOperationTimingVector&& vSrc)
// {
// 	std::cout << "operator = (TSOperationTimingVector&& vSrc)" << std::endl;
// 	m_vOperationTimings = std::move(vSrc.m_vOperationTimings);
// 	vSrc.m_vOperationTimings.clear();
// }
// 
// void TSOperationTimingVector::dump(size_t iBegin, size_t iEnd) 
// {
// 	size_t nTSOpTimings = m_vOperationTimings.size();
// 	if (!nTSOpTimings || iEnd <= iBegin && iEnd)
// 		return;
// 	
// 	if (iEnd == 0)
// 		iEnd = nTSOpTimings - iBegin;
// 
// 	std::cout << std::endl;
// 	
// 	for(; iBegin != iEnd; ++iBegin)
// 		m_vOperationTimings[iBegin]->dump(std::cout);
// 	
// 	std::cout << std::endl;
// }
// 
// void TSOperationTimingVector::clear()
// {
// 	size_t nTSOpTimings = m_vOperationTimings.size();
// 	for(size_t i = 0; i < nTSOpTimings; ++i)
// 		delete m_vOperationTimings[i];
// 	
// 	m_vOperationTimings.clear();
// }
// 
// TSOperationTimingVector TSOperationTimingVector::filterByType(OperationType kOperationType) const
// {
// 	TSOperationTimingVector oResult;
// 	size_t nTSOpTimings = m_vOperationTimings.size();
// 	for(size_t i = 0; i < nTSOpTimings; ++i)
// 	{
// 		if (m_vOperationTimings[i]->m_kOperationType & kOperationType)
// 			oResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
// 	}
// 	
// 	return oResult;
// }
// 
// TSOperationTimingVector TSOperationTimingVector::filterParallelWorkers(const OperationTimingInt* oOpTiming) const
// {
// 	TSOperationTimingVector oResult;
// 	size_t nTSOpTimings = m_vOperationTimings.size();
// 	for(size_t i = 0; i < nTSOpTimings; ++i)
// 	{
// 		if (
// 			m_vOperationTimings[i]->m_tStartTime < oOpTiming->m_tEndTime || 
// 			m_vOperationTimings[i]->m_tEndTime > oOpTiming->m_tStartTime
// 		)
// 			oResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
// 	}
// 	
// 	return oResult;
// }
// 
// TSOperationTimingVector TSOperationTimingVector::filterParallelWorkersByType(
// 	const OperationTimingInt* 	oOpTiming, 
// 	OperationType 				kOperationType
// ) const {
// 	TSOperationTimingVector oResult;
// 	size_t nTSOpTimings = m_vOperationTimings.size();
// 	size_t j = 0;
// 	for(size_t i = 0; i < nTSOpTimings; ++i)
// 	{
// 		if (
// 			m_vOperationTimings[i]->m_kOperationType & kOperationType &&
// 			(
// 				m_vOperationTimings[i]->m_tStartTime < oOpTiming->m_tEndTime || 
// 				m_vOperationTimings[i]->m_tEndTime > oOpTiming->m_tStartTime
// 			)
// 		) {
// 			oResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
// 			oResult[j]->dump(std::cout);
// 			++j;
// 		}
// 	}
// 	
// 	return oResult;
// }
// 
// void TSOperationTimingVector::sortByEndTimeAscAndCalcLockTimes()
// {
// 	std::sort(
// 		m_vOperationTimings.begin(), 
// 		m_vOperationTimings.end(),
// 		[] (const OperationTimingInt* a, const OperationTimingInt* b) { return (a->m_tEndTime < b->m_tEndTime); }
// 	);
// 	
// 	size_t n = m_vOperationTimings.size();
// 	
// 	OperationTimingInt* pLastExclusiveLocker;
// 	if (n) {
// 		m_vOperationTimings[0]->m_tLockTime = m_vOperationTimings[0]->m_tStartTime;
// 		pLastExclusiveLocker = m_vOperationTimings[0];
// 	}
// 	for(size_t i = 1; i < n; ++i) {
// 		if (m_vOperationTimings[i]->m_kOperationType & WRITE_OPS) {
// 			m_vOperationTimings[i]->m_tLockTime = m_vOperationTimings[i - 1]->m_tEndTime + OperationTimingInt::DurationType(1);
// 			pLastExclusiveLocker = m_vOperationTimings[i];
// 		} else {
// 			m_vOperationTimings[i]->m_tLockTime = pLastExclusiveLocker->m_tEndTime + OperationTimingInt::DurationType(1);
// 		}
// 	}
// }
// 
// //  +===========================================================================================
// // DATA DUMPERS +===============================================================================
// //  +===========================================================================================
// 
// #define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"
// 
// OperationTimingInt::DurationType::rep opTimePointToInteger(const OperationTimingInt::TimeType& tTime)
// {
// 	return std::chrono::duration_cast<OperationTimingInt::DurationType>(tTime.time_since_epoch()).count();
// }
// 
// std::ostream& OperationTimingInt::operator << (std::ostream& stream) const
// {	
// 	return dump(stream);
// }
// 
// std::ostream& OperationTimingInt::dump(std::ostream& stream) const
// {
// 	stream 	<< " StartTime: " << opTimePointToInteger(m_tStartTime)
// 			<< ", LockTime: " << opTimePointToInteger(m_tLockTime)
// 			<< ", EndTime: " << opTimePointToInteger(m_tEndTime);
// 			
// 	return stream;
// }
// 
// std::ostream& FindOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'FIND', ";
// 	(OperationTimingInt::dump(stream));
// 	stream	<< ", Key=" << m_tKey 
// 			<< ", Pos=" <<  m_iPos
// 			<< ", Result=" << COUT_PAIR(m_oResult)
// 			<< std::endl;
// 
// 	return stream;
// }
// 
// std::ostream& EqualRangeOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EQUAL_RANGE', ";
// 	OperationTimingInt::dump(stream);
// 	stream	<< ", Key=" << m_tKey 
// 			<< ", Pos=" <<  m_iPos
// 			<< ", Count=" << m_nCount
// 			<< ", Len(Result)=" << m_lResult.size()
// 			<< std::endl;
// 
// 	return stream;
// }
// 
// std::ostream& EqualRangeQOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EQUAL_RANGE_Q []', ";
// 	OperationTimingInt::dump(stream);
// 	stream	<< ", Key=" << m_tKey 
// 			<< ", Len(Result)=" << m_lResult.size()
// 			<< std::endl;
// 
// 	return stream;
// }
// 
// std::ostream& InsertOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'INSERT', "; 
// 	OperationTimingInt::dump(stream);
// 	stream	<< ", Key=" << m_tKey 
// 			<< ", Value=" << m_tValue
// 			<< std::endl;
// 
// 	return stream;
// }
// 
// std::ostream& RemoveOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'REMOVE', "; 
// 	OperationTimingInt::dump(stream);
// 	stream	<< ", Key=" << m_tKey 
// 			<< ", Pos=" <<  m_iPos
// 			<< ", Count=" << m_nCount
// 			<< ", Len(Result)=" << m_lResult.size()
// 			<< std::endl;
// 
// 	return stream;
// }
// 
// std::ostream& ExtendOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EXTEND', "; 
// 	OperationTimingInt::dump(stream);
// 	stream 	<< "NewCapacity=" << m_nNewCapacity << std::endl; 
// 
// 	return stream;
// }
// 
// std::ostream& ClearOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EXTEND', "; 
// 	OperationTimingInt::dump(stream);
// 	stream 	<< "Capacity=" << m_nNewCapacity 
// 			<< ", Size=" << m_nNewSize 
// 			<< std::endl; 
// 	
// 	return stream;
// }
// std::ostream& SizeOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'SIZE', "; 
// 	OperationTimingInt::dump(stream);
// 	stream 	<< "Size=" << m_nTSOpTimings << std::endl; 
// 
// 	return stream;
// }
// 
// std::ostream& EmptyOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EMPTY', "; 
// 	OperationTimingInt::dump(stream);
// 	stream 	<< "bIsEmpty=" << m_bIsEmpty << std::endl; 
// 
// 	return stream;
// }
// 
// std::ostream& CapacityOperationTiming::dump(std::ostream& stream) const 
// {
// 	stream 	<< "OpTiming Type: 'EXTEND', "; 
// 	OperationTimingInt::dump(stream);
// 	stream 	<< "Capacity=" << m_nCapacity << std::endl; 
// 
// 	return stream;
// }
// 
// #undef COUT_PAIR
// 
// //  +===========================================================================================
// // WORKERS +====================================================================================
// //  +===========================================================================================
// 
// // typedef ThreadSafetyTestableHashMap<stdHashFunction> HashMapInt; 
// 
// void findHashMapWorker(HashMapInt& oHashMap, int tKey, size_t iPos, TSOperationTimingVector& vOut)
// {
// 	FindOperationTiming* pFindOperationTiming = new FindOperationTiming();
// 	pFindOperationTiming->m_tKey = tKey;
// 	pFindOperationTiming->m_iPos = iPos;
// 	pFindOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pFindOperationTiming->m_oResult = oHashMap.find(tKey, PairType(12345678, 12345678), iPos);
// 	
// 	vOut.m_mMutex.lock();
// 	pFindOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pFindOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void equalRangeHashMapWorker(HashMapInt& oHashMap, int tKey, size_t iPos, size_t nCount, TSOperationTimingVector& vOut)
// {
// 	EqualRangeOperationTiming* pEqualRangeOperationTiming = new EqualRangeOperationTiming();
// 	pEqualRangeOperationTiming->m_tKey = tKey;
// 	pEqualRangeOperationTiming->m_iPos = iPos;
// 	pEqualRangeOperationTiming->m_nCount = nCount;
// 	pEqualRangeOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	oHashMap.equalRange(tKey, iPos, nCount);
// 	
// 	vOut.m_mMutex.lock();
// 	pEqualRangeOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pEqualRangeOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void equalRangeQHashMapWorker(HashMapInt& oHashMap, int tKey, TSOperationTimingVector& vOut)
// {
// 	EqualRangeQOperationTiming* pEqualRangeQOperationTiming = new EqualRangeQOperationTiming();
// 	pEqualRangeQOperationTiming->m_tKey = tKey;
// 	pEqualRangeQOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pEqualRangeQOperationTiming->m_lResult = oHashMap[tKey];
// 	
// 	vOut.m_mMutex.lock();
// 	pEqualRangeQOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pEqualRangeQOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void insertHashMapWorker(HashMapInt& oHashMap, int tKey, int tValue, TSOperationTimingVector& vOut)
// {
// 	InsertOperationTiming* pInsertOperationTiming = new InsertOperationTiming();
// 	pInsertOperationTiming->m_tKey = tKey;
// 	pInsertOperationTiming->m_tValue = tValue;
// 	pInsertOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 
// 	oHashMap.insert(tKey, tValue);
// 	
// 	vOut.m_mMutex.lock();
// 	pInsertOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pInsertOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void removeHashMapWorker(
// 	HashMapInt&									oHashMap,
// 	const typename HashMapInt::HashFuncArgType 	tKey,
// 	size_t 										iPos,
// 	size_t										nCount,
// 	TSOperationTimingVector&					vOut
// ) {
// 	RemoveOperationTiming* pRemoveOperationTiming = new RemoveOperationTiming();
// 	pRemoveOperationTiming->m_tKey = tKey;
// 	pRemoveOperationTiming->m_iPos = iPos;
// 	pRemoveOperationTiming->m_nCount = nCount;
// 	pRemoveOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pRemoveOperationTiming->m_lResult = oHashMap.remove(tKey, iPos, nCount);
// 	
// 	vOut.m_mMutex.lock();
// 	pRemoveOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pRemoveOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void extendHashMapWorker(HashMapInt& oHashMap, const size_t nNewCapacity, TSOperationTimingVector& vOut)
// {
// 	ExtendOperationTiming* pExtendOperationTiming = new ExtendOperationTiming();
// 	pExtendOperationTiming->m_nNewCapacity = nNewCapacity;
// 	pExtendOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	oHashMap.extend(nNewCapacity);
// 	
// 	vOut.m_mMutex.lock();
// 	pExtendOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pExtendOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void sizeHashMapWorker(HashMapInt& oHashMap, TSOperationTimingVector& vOut) {
// 	SizeOperationTiming* pSizeOperationTiming = new SizeOperationTiming;
// 	pSizeOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pSizeOperationTiming->m_nTSOpTimings = oHashMap.size();
// 	
// 	vOut.m_mMutex.lock();
// 	pSizeOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pSizeOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void emptyHashMapWorker(HashMapInt& oHashMap, TSOperationTimingVector& vOut) {
// 	EmptyOperationTiming* pEmptyOperationTiming = new EmptyOperationTiming();
// 	pEmptyOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pEmptyOperationTiming->m_bIsEmpty = oHashMap.empty();
// 	
// 	vOut.m_mMutex.lock();
// 	pEmptyOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pEmptyOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void capacityHashMapWorker(HashMapInt& oHashMap, TSOperationTimingVector& vOut) 
// {
// 	CapacityOperationTiming* pCapacityOperationTiming = new CapacityOperationTiming();
// 	pCapacityOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	pCapacityOperationTiming->m_nCapacity = oHashMap.capacity();
// 	
// 	vOut.m_mMutex.lock();
// 	pCapacityOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pCapacityOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// void clearHashMapWorker(HashMapInt& oHashMap, TSOperationTimingVector& vOut)
// {
// 	ClearOperationTiming* pClearOperationTiming = new ClearOperationTiming();
// 	pClearOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
// 	
// 	oHashMap.clear();
// 	
// 	vOut.m_mMutex.lock();
// 	pClearOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
// 	vOut.m_vOperationTimings.push_back(pClearOperationTiming);
// 	vOut.m_mMutex.unlock();
// }
// 
// 
// 
// #endif
