#ifndef THRSF_WRAPPERS_H_
#define THRSF_WRAPPERS_H_

#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>
#include <algorithm>

#include "hashmap.h"

//  +===========================================================================================
// DATA STRUCTURES +============================================================================
//  +===========================================================================================

enum OperationType {
	UNKNOWN			= 0,
	
	FIND 			= 1,
	EQUAL_RANGE		= FIND << 1,
	EQUAL_RANGE_Q	= EQUAL_RANGE << 1,
	SIZE			= EQUAL_RANGE_Q << 1,
	CAPACITY		= SIZE << 1,
	EMPTY			= CAPACITY << 1,
	
	INSERT			= EMPTY << 1,
	REMOVE			= INSERT << 1,
	EXTEND			= REMOVE << 1,
	CLEAR			= EXTEND << 1,
	
	FIND_OPS 		= FIND | EQUAL_RANGE | EQUAL_RANGE_Q,
	KEYED_OPS 		= FIND_OPS | INSERT | REMOVE,
	POSITIONAL_OPS	= FIND | EQUAL_RANGE | REMOVE,
	RANGE_RES_OPS	= EQUAL_RANGE | EQUAL_RANGE_Q | REMOVE,
	COUNTED_OPS		= EQUAL_RANGE | REMOVE,
	
	READ_OPS		= FIND_OPS | SIZE | CAPACITY | EMPTY,
	WRITE_OPS		= INSERT | REMOVE | EXTEND | CLEAR
};

//  +===========================================================================================
// RESULTS STRUCTURES +=========================================================================
//  +===========================================================================================

template <
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<int>::HashFuncArgType, size_t) = stdHashFunction
>
class ThreadSafetyTestableHashMap;

typedef HashMap<int, int>				HashMapInt;
typedef HashMapInt::PairType			PairType;
typedef HashMapInt::EqualRangeType		EqualRangeType;

struct OperationTimingInt 
{
	typedef std::chrono::high_resolution_clock 					ClockType;
	typedef std::chrono::nanoseconds 							DurationType;
	typedef std::chrono::time_point<ClockType>					TimeType;
	
	OperationType			m_kOperationType;
	
	TimeType				m_tStartTime;
	TimeType				m_tLockTime;
	TimeType				m_tEndTime;
	
	OperationTimingInt(OperationType kOperationType) : m_kOperationType(kOperationType) {}
	virtual ~OperationTimingInt() {}
	
	virtual std::ostream& dump(std::ostream& stream) const;
};

struct IFindOpsTiming : OperationTimingInt
{
	int					m_tKey;
	
	IFindOpsTiming(OperationType kOperationType) : OperationTimingInt(kOperationType) {}
	virtual ~IFindOpsTiming() {}
};

struct FindOperationTiming : IFindOpsTiming {
	size_t				m_iPos;
	size_t				m_nCount;
	PairType			m_oResult;
	
	FindOperationTiming() : IFindOpsTiming(FIND) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct EqualRangeOperationTiming : IFindOpsTiming {
	size_t				m_iPos;
	size_t				m_nCount;
	EqualRangeType		m_lResult;
	
	EqualRangeOperationTiming() : IFindOpsTiming(EQUAL_RANGE) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct EqualRangeQOperationTiming : IFindOpsTiming {
	EqualRangeType		m_lResult;
	
	EqualRangeQOperationTiming() : IFindOpsTiming(EQUAL_RANGE_Q) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct InsertOperationTiming : OperationTimingInt {
	int					m_tKey;
	int					m_tValue;
	
	InsertOperationTiming() : OperationTimingInt(INSERT) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct RemoveOperationTiming : OperationTimingInt {
	int					m_tKey;
	size_t				m_iPos;
	size_t				m_nCount;
	EqualRangeType		m_lResult;
	
	RemoveOperationTiming() : OperationTimingInt(REMOVE) {}
	
	std::ostream& dump(std::ostream& stream) const;
};
// 
struct ExtendOperationTiming : OperationTimingInt {
	size_t 				m_nNewCapacity;
	
	ExtendOperationTiming() : OperationTimingInt(EXTEND) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct SizeOperationTiming : OperationTimingInt {
	size_t 				m_nSize;
	
	SizeOperationTiming() : OperationTimingInt(SIZE) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct CapacityOperationTiming : OperationTimingInt {
	size_t 				m_nCapacity;
	
	CapacityOperationTiming() : OperationTimingInt(CAPACITY) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct EmptyOperationTiming : OperationTimingInt {
	bool 				m_bIsEmpty;
	
	EmptyOperationTiming() : OperationTimingInt(EMPTY) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

struct ClearOperationTiming : OperationTimingInt {
	size_t				m_nNewCapacity;
	size_t				m_nNewSize;
	
	ClearOperationTiming() : OperationTimingInt(CLEAR) {}
	
	std::ostream& dump(std::ostream& stream) const;
};

//  +===========================================================================================
// RESULTS STRUCTURES RANGE INTERFACE +=========================================================
//  +=========================================================================================== 

struct TSOperationTimingVector {
	std::vector<OperationTimingInt*>	m_vOperationTimings;
	std::mutex							m_mMutex;
	bool								m_bIsSorted;

	TSOperationTimingVector() {}
	TSOperationTimingVector(const TSOperationTimingVector& vSrc);
	TSOperationTimingVector(TSOperationTimingVector&& vSrc);
	~TSOperationTimingVector() { clear(); }
	
	void dump(size_t iBegin=0, size_t iEnd=0);
	void clear();
	void unlink();
	
	TSOperationTimingVector filterByType(OperationType kOperationType) const;
	TSOperationTimingVector filterParallelWorkers(const OperationTimingInt* oOpTiming) const;
	TSOperationTimingVector filterParallelWorkersByType(const OperationTimingInt* oOpTiming, OperationType kOperationType) const;
	
	TSOperationTimingVector& operator = (const TSOperationTimingVector& vSrc);
	TSOperationTimingVector& operator = (TSOperationTimingVector&& vSrc);
	
	const OperationTimingInt* operator [] (size_t i) const { return m_vOperationTimings[i]; } 
	size_t size() const { return m_vOperationTimings.size(); }
	
	std::vector<OperationTimingInt*>::iterator begin() { return m_vOperationTimings.begin(); }
	std::vector<OperationTimingInt*>::iterator end() { return m_vOperationTimings.end(); }
	
	void sortByEndTimeAsc();
};


TSOperationTimingVector::TSOperationTimingVector(const TSOperationTimingVector& vSrc)
{
	m_bIsSorted = vSrc.m_bIsSorted;
	m_vOperationTimings = vSrc.m_vOperationTimings;
}

TSOperationTimingVector::TSOperationTimingVector(TSOperationTimingVector&& vSrc)
{
	m_bIsSorted = vSrc.m_bIsSorted;
	m_vOperationTimings = std::move(vSrc.m_vOperationTimings);
	vSrc.m_vOperationTimings.clear();
	vSrc.m_bIsSorted = false;
}

TSOperationTimingVector& TSOperationTimingVector::operator = (const TSOperationTimingVector& vSrc)
{
	m_vOperationTimings = vSrc.m_vOperationTimings;
	m_bIsSorted = vSrc.m_bIsSorted;
}

TSOperationTimingVector& TSOperationTimingVector::operator = (TSOperationTimingVector&& vSrc)
{
	m_vOperationTimings = std::move(vSrc.m_vOperationTimings);
	vSrc.m_vOperationTimings.clear();
	
	m_bIsSorted = vSrc.m_bIsSorted;
	vSrc.m_bIsSorted = false;
}

void TSOperationTimingVector::dump(size_t iBegin, size_t iEnd) 
{
	size_t nSize = m_vOperationTimings.size();
	if (!nSize || iEnd <= iBegin && iEnd)
		return;
	
	if (iEnd == 0)
		iEnd = nSize - iBegin;

	std::cout << std::endl;
	
	for(; iBegin != iEnd; ++iBegin)
		m_vOperationTimings[iBegin]->dump(std::cout);
	
	std::cout << std::endl;
}

void TSOperationTimingVector::clear()
{
	size_t nSize = m_vOperationTimings.size();
	for(size_t i = 0; i < nSize; ++i) 
		delete m_vOperationTimings[i];
	
	m_vOperationTimings.clear();
}

void TSOperationTimingVector::unlink()
{
	m_vOperationTimings.clear();
}

TSOperationTimingVector TSOperationTimingVector::filterByType(OperationType kOperationType) const
{
	TSOperationTimingVector vResult;
	size_t nSize = m_vOperationTimings.size();
	for(size_t i = 0; i < nSize; ++i)
	{
		if (m_vOperationTimings[i]->m_kOperationType & kOperationType)
			vResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
	}
	
	if (!m_bIsSorted)
		vResult.sortByEndTimeAsc();
	
	return vResult;
}

TSOperationTimingVector TSOperationTimingVector::filterParallelWorkers(const OperationTimingInt* oOpTiming) const
{
	TSOperationTimingVector vResult;
	size_t nSize = m_vOperationTimings.size();
	for(size_t i = 0; i < nSize; ++i)
	{
		if (
			m_vOperationTimings[i] != oOpTiming &&
			( 
				m_vOperationTimings[i]->m_tStartTime < oOpTiming->m_tEndTime || 
				m_vOperationTimings[i]->m_tEndTime > oOpTiming->m_tStartTime
			)
		)
			vResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
	}
	
	if (!m_bIsSorted)
		vResult.sortByEndTimeAsc();
	
	return vResult;
}

TSOperationTimingVector TSOperationTimingVector::filterParallelWorkersByType(
	const OperationTimingInt* 	oOpTiming, 
	OperationType 				kOperationType
) const {
	TSOperationTimingVector vResult;
	size_t nSize = m_vOperationTimings.size();
	for(size_t i = 0; i < nSize; ++i)
	{
		if (
			m_vOperationTimings[i] != oOpTiming &&
			m_vOperationTimings[i]->m_kOperationType & kOperationType &&
			(
				m_vOperationTimings[i]->m_tStartTime < oOpTiming->m_tEndTime || 
				m_vOperationTimings[i]->m_tEndTime > oOpTiming->m_tStartTime
			)
		) {
			vResult.m_vOperationTimings.push_back(m_vOperationTimings[i]);
		}
	}
	
	if (!m_bIsSorted)
		vResult.sortByEndTimeAsc();
	
	return vResult;
}

void TSOperationTimingVector::sortByEndTimeAsc()
{
	if (!m_bIsSorted) {
		std::sort(
			m_vOperationTimings.begin(), 
			m_vOperationTimings.end(),
			[] (const OperationTimingInt* a, const OperationTimingInt* b) { return (a->m_tEndTime < b->m_tEndTime); }
		);
		m_bIsSorted = true;
	}
}

//  +===========================================================================================
// THREAD SAFETY TESTABLE HASH MAP CLASS +======================================================
//  +===========================================================================================

#ifndef TST_HASHMAP_H_
#define TST_HASHMAP_H_

#include <vector>
#include <cstring>

#include "hashmap.h"

#define HM_TEMPLATE_DECL \
template <\
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<int>::HashFuncArgType, size_t)\
> 

#define TThreadSafetyTestableHashMap ThreadSafetyTestableHashMap<hashFunction>

#define key(pair) pair.first
#define value(pair) pair.second

#define key_ptr(ptr) ptr->first
#define value_ptr(ptr) ptr->second

#define cell_is_free(cell) !cell
#define cell_is_busy(cell) cell

#define is_valid_hash(h) h < BaseHashMap::m_nCapacity
#define is_invalid_hash(h) h >= BaseHashMap::m_nCapacity

#include <vector>
#include <cstring>

#include "hashmap.h"

#define DEFAULT_CAPACITY 127 

template <
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<int>::HashFuncArgType, size_t)
>
class ThreadSafetyTestableHashMap : public HashMap<int, int, hashFunction>
{
public:
	typedef HashMap<int, int, hashFunction>					BaseHashMap;
	
	typedef typename BaseHashMap::KeyTypeTraits				KeyTypeTraits;			
	typedef typename KeyTypeTraits::HashFuncArgType 		HashFuncArgType;
	
	typedef typename BaseHashMap::PairType 					PairType;
	typedef typename BaseHashMap::RefPairType				RefPairType;
	typedef typename BaseHashMap::ConstRefPairType			ConstRefPairType;
	
	typedef typename BaseHashMap::RefType					RefType;
	typedef typename BaseHashMap::ConstRefType				ConstRefType;
	
	typedef typename BaseHashMap::SharedPtrPairType			SharedPtrPairType;
	typedef typename BaseHashMap::ConstSharedPtrPairType	ConstSharedPtrPairType;
	typedef typename BaseHashMap::WeakPtrPairType			WeakPtrPairType;
	typedef typename BaseHashMap::ConstWeakPtrPairType		ConstWeakPtrPairType;
	typedef typename BaseHashMap::ConstRefWeakPtrPairType	ConstRefWeakPtrPairType;
	
	typedef typename BaseHashMap::EqualRangeType 			EqualRangeType;
	
	typedef typename BaseHashMap::ReadLockType				ReadLockType;
	typedef typename BaseHashMap::WriteLockType				WriteLockType;
	
	typedef typename BaseHashMap::HashTableType				HashTableType;
	
public:
	ThreadSafetyTestableHashMap() : BaseHashMap::HashMap() {};
	ThreadSafetyTestableHashMap(const std::initializer_list<PairType>& lInit) : BaseHashMap::HashMap(lInit) {}
	ThreadSafetyTestableHashMap(const HashMap<int, int, hashFunction>& oSrc) : BaseHashMap::HashMap(oSrc) {}
	ThreadSafetyTestableHashMap(HashMap<int, int, hashFunction>&& oSrc) : BaseHashMap::HashMap(oSrc) {}
	
public:
	mutable TSOperationTimingVector 	m_vTSOpTimings;
	
	ConstRefPairType find(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	EqualRangeType operator [] (HashFuncArgType tKey) const;
	EqualRangeType equalRange(HashFuncArgType tKey, size_t iPos=0, size_t nCount=0) const;
	
	void insert(HashFuncArgType tKey, ConstRefType tValue);
	
	EqualRangeType remove(HashFuncArgType tKey, size_t iPos = 0, size_t nCount = 0);
	
	void extend(const size_t nNewCapacity);
	
	bool 	empty() const;
	size_t	capacity() const;
	size_t 	size() const;
	
	void clear();
};

#define HM_TEMPLATE_DECL \
template <\
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<int>::HashFuncArgType, size_t)\
> 

#define THashMap ThreadSafetyTestableHashMap<hashFunction>

#define key(pair) pair.first
#define value(pair) pair.second

#define key_ptr(ptr) ptr->first
#define value_ptr(ptr) ptr->second

#define cell_is_free(cell) !cell
#define cell_is_busy(cell) cell

#define is_valid_hash(h) h < BaseHashMap::m_nCapacity
#define is_invalid_hash(h) h >= BaseHashMap::m_nCapacity

HM_TEMPLATE_DECL
typename THashMap::ConstRefPairType 
THashMap::find(THashMap::HashFuncArgType tKey, THashMap::ConstRefPairType oDefaultPair, size_t iPos) const 
{
	FindOperationTiming* pFindOperationTiming = new FindOperationTiming();
	pFindOperationTiming->m_tKey = tKey;
	pFindOperationTiming->m_iPos = iPos;
	pFindOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType	lReadLock(BaseHashMap::m_mSharedMutex);
	pFindOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	pFindOperationTiming->m_oResult = BaseHashMap::findImpl(tKey, oDefaultPair, iPos);
	
	m_vTSOpTimings.m_mMutex.lock();
	pFindOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pFindOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
	
	return pFindOperationTiming->m_oResult;
}

HM_TEMPLATE_DECL
typename THashMap::EqualRangeType 
THashMap::equalRange(const typename THashMap::HashFuncArgType tKey, size_t iPos, size_t nCount) const 
{	
	EqualRangeOperationTiming* pEqualRangeOperationTiming = new EqualRangeOperationTiming();
	pEqualRangeOperationTiming->m_tKey = tKey;
	pEqualRangeOperationTiming->m_iPos = iPos;
	pEqualRangeOperationTiming->m_nCount = nCount;
	
	pEqualRangeOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType	lReadLock(BaseHashMap::m_mSharedMutex);
	pEqualRangeOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	if (!iPos && !nCount) {
		BaseHashMap::equalRangeImpl(tKey, pEqualRangeOperationTiming->m_lResult);
	} else {
		BaseHashMap::equalRangeImplForPos(tKey, iPos, nCount, pEqualRangeOperationTiming->m_lResult);
	}
	
	m_vTSOpTimings.m_mMutex.lock();
	pEqualRangeOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pEqualRangeOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
		
	return pEqualRangeOperationTiming->m_lResult;
}

HM_TEMPLATE_DECL 
typename THashMap::EqualRangeType 
THashMap::operator [] (const typename THashMap::HashFuncArgType tKey) const 
{
	EqualRangeQOperationTiming* pEqualRangeQOperationTiming = new EqualRangeQOperationTiming();
	pEqualRangeQOperationTiming->m_tKey = tKey;
	pEqualRangeQOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType	lReadLock(BaseHashMap::m_mSharedMutex);
	pEqualRangeQOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	BaseHashMap::equalRangeImpl(tKey, pEqualRangeQOperationTiming->m_lResult);
	
	m_vTSOpTimings.m_mMutex.lock();
	pEqualRangeQOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pEqualRangeQOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
	
	return pEqualRangeQOperationTiming->m_lResult;
}

HM_TEMPLATE_DECL  
void THashMap::insert(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefType tValue)
{
	InsertOperationTiming* pInsertOperationTiming = new InsertOperationTiming();
	pInsertOperationTiming->m_tKey = tKey;
	pInsertOperationTiming->m_tValue = tValue;
	pInsertOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();

	WriteLockType	lWriteLock(BaseHashMap::m_mSharedMutex);
	pInsertOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	BaseHashMap::insertImpl(tKey, tValue);
	
	m_vTSOpTimings.m_mMutex.lock();
	pInsertOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pInsertOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
}

HM_TEMPLATE_DECL
typename THashMap::EqualRangeType 
THashMap::remove(
	const typename THashMap::HashFuncArgType 	tKey,
	size_t 										iPos,
	size_t										nCount
) {
	RemoveOperationTiming* pRemoveOperationTiming = new RemoveOperationTiming();
	pRemoveOperationTiming->m_tKey = tKey;
	pRemoveOperationTiming->m_iPos = iPos;
	pRemoveOperationTiming->m_nCount = nCount;
	pRemoveOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	WriteLockType	lWriteLock(BaseHashMap::m_mSharedMutex);
	pRemoveOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	BaseHashMap::removeImpl(tKey, iPos, nCount, pRemoveOperationTiming->m_lResult);
	
	m_vTSOpTimings.m_mMutex.lock();
	pRemoveOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pRemoveOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
	
	return pRemoveOperationTiming->m_lResult;
}

HM_TEMPLATE_DECL
void THashMap::extend(const size_t nNewCapacity)
{
	ExtendOperationTiming* pExtendOperationTiming = new ExtendOperationTiming();
	pExtendOperationTiming->m_nNewCapacity = nNewCapacity;
	pExtendOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	WriteLockType	lWriteLock(BaseHashMap::m_mSharedMutex);
	pExtendOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	BaseHashMap::extendImpl(nNewCapacity);
	
	m_vTSOpTimings.m_mMutex.lock();
	pExtendOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pExtendOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
}

HM_TEMPLATE_DECL
size_t THashMap::size() const {
	SizeOperationTiming* pSizeOperationTiming = new SizeOperationTiming;
	pSizeOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType lReadLock(BaseHashMap::m_mSharedMutex);
	pSizeOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	pSizeOperationTiming->m_nSize = BaseHashMap::sizeImpl();
	
	m_vTSOpTimings.m_mMutex.lock();
	pSizeOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pSizeOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
	
	return pSizeOperationTiming->m_nSize;
}

HM_TEMPLATE_DECL
bool THashMap::empty() const {
	EmptyOperationTiming* pEmptyOperationTiming = new EmptyOperationTiming();
	pEmptyOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType lReadLock(BaseHashMap::m_mSharedMutex);
	pEmptyOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	pEmptyOperationTiming->m_bIsEmpty = BaseHashMap::emptyImpl();
	
	m_vTSOpTimings.m_mMutex.lock();
	pEmptyOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pEmptyOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();

	return pEmptyOperationTiming->m_bIsEmpty;
}

HM_TEMPLATE_DECL
size_t THashMap::capacity() const 
{
	CapacityOperationTiming* pCapacityOperationTiming = new CapacityOperationTiming();
	pCapacityOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	ReadLockType	lReadLock(BaseHashMap::m_mSharedMutex);
	pCapacityOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	pCapacityOperationTiming->m_nCapacity = BaseHashMap::m_nCapacity;
	
	m_vTSOpTimings.m_mMutex.lock();
	pCapacityOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pCapacityOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
	
	return BaseHashMap::m_nCapacity;
}

HM_TEMPLATE_DECL void THashMap::clear()
{
	ClearOperationTiming* pClearOperationTiming = new ClearOperationTiming();
	pClearOperationTiming->m_tStartTime = OperationTimingInt::ClockType::now();
	
	WriteLockType lWriteLock(BaseHashMap::m_mSharedMutex);
	pClearOperationTiming->m_tLockTime = OperationTimingInt::ClockType::now();
	
	BaseHashMap::clearImpl();
	
	pClearOperationTiming->m_nNewCapacity = BaseHashMap::m_nCapacity;	
	pClearOperationTiming->m_nNewSize = BaseHashMap::sizeImpl();
	
	m_vTSOpTimings.m_mMutex.lock();
	pClearOperationTiming->m_tEndTime = OperationTimingInt::ClockType::now();
	m_vTSOpTimings.m_vOperationTimings.push_back(pClearOperationTiming);
	m_vTSOpTimings.m_mMutex.unlock();
}

#undef key
#undef value
#undef key_ptr
#undef value_ptr
#undef cell_is_free
#undef cell_is_busy
#undef is_invalid_hash
#undef is_valid_hash
#undef HM_TEMPLATE_DECL
#undef THashMap
#undef DEFAULT_CAPACITY

#endif


//  +===========================================================================================
// DATA DUMPERS +===============================================================================
//  +===========================================================================================

#define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"

OperationTimingInt::DurationType::rep opTimePointToInteger(const OperationTimingInt::TimeType& tTime)
{
	return std::chrono::duration_cast<OperationTimingInt::DurationType>(tTime.time_since_epoch()).count();
}

std::ostream& OperationTimingInt::dump(std::ostream& stream) const
{
	stream 	<< " StartTime: " << opTimePointToInteger(m_tStartTime)
			<< ", LockTime: " << opTimePointToInteger(m_tLockTime)
			<< ", EndTime: " << opTimePointToInteger(m_tEndTime);
			
	return stream;
}

std::ostream& FindOperationTiming::dump(std::ostream& stream) const
{
	stream 	<< "OpTiming Type: 'FIND', ";
	(OperationTimingInt::dump(stream));
	stream	<< ", Key=" << m_tKey 
			<< ", Pos=" <<  m_iPos
			<< ", Result=" << COUT_PAIR(m_oResult)
			<< std::endl;

	return stream;
}

std::ostream& EqualRangeOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EQUAL_RANGE', ";
	OperationTimingInt::dump(stream);
	stream	<< ", Key=" << m_tKey 
			<< ", Pos=" <<  m_iPos
			<< ", Count=" << m_nCount
			<< ", Len(Result)=" << m_lResult.size()
			<< std::endl;

	return stream;
}

std::ostream& EqualRangeQOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EQUAL_RANGE_Q []', ";
	OperationTimingInt::dump(stream);
	stream	<< ", Key=" << m_tKey 
			<< ", Len(Result)=" << m_lResult.size()
			<< std::endl;

	return stream;
}

std::ostream& InsertOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'INSERT', "; 
	OperationTimingInt::dump(stream);
	stream	<< ", Key=" << m_tKey 
			<< ", Value=" << m_tValue
			<< std::endl;

	return stream;
}

std::ostream& RemoveOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'REMOVE', "; 
	OperationTimingInt::dump(stream);
	stream	<< ", Key=" << m_tKey 
			<< ", Pos=" <<  m_iPos
			<< ", Count=" << m_nCount
			<< ", Len(Result)=" << m_lResult.size()
			<< std::endl;

	return stream;
}

std::ostream& ExtendOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EXTEND', "; 
	OperationTimingInt::dump(stream);
	stream 	<< "NewCapacity=" << m_nNewCapacity << std::endl; 

	return stream;
}

std::ostream& ClearOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EXTEND', "; 
	OperationTimingInt::dump(stream);
	stream 	<< "Capacity=" << m_nNewCapacity 
			<< ", Size=" << m_nNewSize 
			<< std::endl; 
	
	return stream;
}
std::ostream& SizeOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'SIZE', "; 
	OperationTimingInt::dump(stream);
	stream 	<< "Size=" << m_nSize << std::endl; 

	return stream;
}

std::ostream& EmptyOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EMPTY', "; 
	OperationTimingInt::dump(stream);
	stream 	<< "bIsEmpty=" << m_bIsEmpty << std::endl; 

	return stream;
}

std::ostream& CapacityOperationTiming::dump(std::ostream& stream) const 
{
	stream 	<< "OpTiming Type: 'EXTEND', "; 
	OperationTimingInt::dump(stream);
	stream 	<< "Capacity=" << m_nCapacity << std::endl; 

	return stream;
}

#undef COUT_PAIR

//  +===========================================================================================
// WORKERS +====================================================================================
//  +===========================================================================================

#define TSTHashMapInt ThreadSafetyTestableHashMap<stdHashFunction>
#define UNUSED_ARG(x) (void)x;

// macro for aligning Workers by time before call checked method
#define NOPE(x) (void)x;

void findHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t nCount)
{
	UNUSED_ARG(nCount)
	NOPE(tKey)
	NOPE(iPos)

	oHashMap.find(tKey, PairType(12345678, 12345678), iPos);
}

void equalRangeHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t nCount)
{
	oHashMap.equalRange(tKey, iPos, nCount);
}

void equalRangeQHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t nCount)
{
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)
	NOPE(tKey)
	
	oHashMap[tKey];
}

void insertHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t nCount)
{
	UNUSED_ARG(nCount)
	NOPE(tKey)
	NOPE(iPos)
	
	oHashMap.insert(tKey, tKey * 10 + iPos);
}

void removeHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount) 
{	
	NOPE(tKey)
	NOPE(iPos)
	NOPE(nCount)
	
	oHashMap.remove(tKey, iPos, nCount);
}

void extendHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount)
{
	UNUSED_ARG(tKey)
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)
	
	oHashMap.extend(oHashMap.defaultCapacity() + oHashMap.capacity());
}

void sizeHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount) {
	UNUSED_ARG(tKey)
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)
	
	oHashMap.size();
}

void emptyHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount) {
	UNUSED_ARG(tKey)
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)	

	oHashMap.empty();
}

void capacityHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount) 
{	
	UNUSED_ARG(tKey)
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)
	
	oHashMap.capacity();
}

void clearHashMapWorker(TSTHashMapInt& oHashMap, int tKey, size_t iPos, size_t	nCount)
{
	UNUSED_ARG(tKey)
	UNUSED_ARG(iPos)
	UNUSED_ARG(nCount)
	
	oHashMap.clear();
}

//  +===========================================================================================
// CHECKERS +===================================================================================
//  +===========================================================================================

bool checkLockTimes(const TSOperationTimingVector& vTSOpTimings)
{
	using namespace std;
	
	bool bResult = true;
	
	size_t 	nTSOpTimings = vTSOpTimings.size(), 
			jLastWrite;
	for(size_t i = 0; i < nTSOpTimings; ++i)
	{
		TSOperationTimingVector vParallels = vTSOpTimings.filterParallelWorkers(vTSOpTimings[i]);
		size_t nParallels = vParallels.size();
		jLastWrite = nParallels;
		
		for(size_t j = 0; j < nParallels; ++j) {
			if (vTSOpTimings[i]->m_kOperationType & WRITE_OPS) 
			{
				if (vParallels[j]->m_kOperationType & WRITE_OPS) {
					if (vParallels[j]->m_tLockTime < vTSOpTimings[i]->m_tLockTime) 
					{
						if (vParallels[j]->m_tEndTime > vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Write Operation (x) has lock BEFORE threated (x0) Write Operation" 
									<< ", but ended AFTER this" << endl;
						}
					} 
					else 
					{
						if (vParallels[j]->m_tEndTime < vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Write Operation (x) has lock AFTER threated (x0) Write Operation" 
									<< ", but ended BEFORE this" << endl;
						}
					}
					
					jLastWrite = j;
				} 
				else if (vParallels[j]->m_kOperationType & READ_OPS) 
				{
					if (vParallels[j]->m_tLockTime < vTSOpTimings[i]->m_tLockTime) 
					{
						if (vParallels[j]->m_tEndTime > vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Read Operation (x) has lock BEFORE threated (x0) Write Operation" 
									<< ", but ended AFTER this" << endl;
						}
					} 
					else 
					{
						if (vParallels[j]->m_tEndTime < vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Read Operation (x) has lock BEFORE threated (x0) Write Operation" 
									<< ", but ended BEFORE this" << endl;
						}
					}
				}
			} 
			else if (vTSOpTimings[i]->m_kOperationType & READ_OPS) 
			{
				if (vParallels[j]->m_kOperationType & WRITE_OPS) {
					if (vParallels[j]->m_tLockTime < vTSOpTimings[i]->m_tLockTime) 
					{
						if (vParallels[j]->m_tEndTime > vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Write Operation (x) has lock BEFORE threated (x0) Read Operation" 
									<< ", but ended AFTER this" << endl;
						}
					} 
					else 
					{
						if (vParallels[j]->m_tEndTime < vTSOpTimings[i]->m_tEndTime) {
							bResult = false;
							cout 	<< " FAIL:    " 
									<< "Write Operation (x) has lock AFTER threated (x0) Read Operation" 
									<< ", but ended BEFORE this" << endl;
						}
					}
					
					jLastWrite = j;
				} 
				else if (vParallels[j]->m_kOperationType & READ_OPS) 
				{
					if (
						vParallels[j]->m_tLockTime < vTSOpTimings[i]->m_tEndTime && 
						vParallels[j]->m_tLockTime > vTSOpTimings[i]->m_tStartTime
					)
						continue;
					
					if (
						jLastWrite != nParallels
					) {
						if (vParallels[j]->m_tLockTime < vTSOpTimings[i]->m_tStartTime) 
						{
							if (vParallels[jLastWrite]->m_tEndTime > vParallels[j]->m_tLockTime) {
								bResult = false;
								cout	<< " FAIL:    " 
										<< "Read Operation (x) has lock BEFORE ending threated (x0) Read Operation" 
										<< ", but lock was given BEFORE ending last Write Operation" << endl;
							}
						}
						else if (vParallels[j]->m_tLockTime > vTSOpTimings[i]->m_tEndTime) 
						{
							if (vParallels[jLastWrite]->m_tEndTime > vParallels[j]->m_tLockTime) {
								bResult = false;
								cout	<< " FAIL:    " 
										<< "Read Operation (x) has lock AFTER ending threated (x0) Read Operation" 
										<< ", but lock was given BEFORE ending last Write Operation" << endl;
							}
						} 
					}
				}
			}
		}
		
		vParallels.unlink();
	}
	
	return bResult;
}

#define _IN
#define _OUT
#define _INOUT

typedef std::list<std::thread>			ThreadList;

template <typename WriteWorkerFunction>
void runWriteAndFindThreads(
	_INOUT TSTHashMapInt& 	oHashMap, 
	_IN int 				iStartKey, 
	_IN int 				iEndKey, 
	_IN size_t 				nCount,
	_IN WriteWorkerFunction	writeThreadWorker
) {
	ThreadList lThreads;
	std::random_device oRandom;
	TSTHashMapInt::PairType oDefaultPair(12345678, 12345678);
	// run
	for(int iKey = iStartKey; iKey <= iEndKey; ++iKey) 
	{
		size_t iPos = 0, iPrevPos;
		bool bEnd = false;
		while(iPos < nCount)
		{
			iPrevPos = (iPos) ? iPos - 1 : iPos;
			lThreads.push_back(
				std::thread(findHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount)
			);
			lThreads.push_back(
				std::thread(equalRangeHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount)
			);
			lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
			
			lThreads.push_back(std::thread(writeThreadWorker, std::ref(oHashMap), iKey, iPos, nCount));
			
			lThreads.push_back(std::thread(findHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
			lThreads.push_back(std::thread(equalRangeHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
			lThreads.push_back(std::thread(equalRangeQHashMapWorker, std::ref(oHashMap), iKey, iPos, nCount));
			
			++iPos;
		}
	}
	
	ThreadList::iterator it = lThreads.begin(), it_end = lThreads.end();
	for(; it != it_end; ++it) 
		it->join();
}

typedef unsigned char byte;

struct CheckReadersFlags
{
	enum Flags {
		kReadBeforeResult			= 1,
		kReadAfterResult			= kReadBeforeResult << 1,
		kWriterHasNoReadersAfter	= kReadAfterResult << 1
	};
	
	byte m_kFlags;
	
	CheckReadersFlags() : m_kFlags(kReadBeforeResult | kWriterHasNoReadersAfter) {}
	
	operator bool () {
		return (m_kFlags & (kReadBeforeResult | kReadAfterResult)) || (m_kFlags & (kReadBeforeResult | kWriterHasNoReadersAfter));
	}
};

#define COUT_PAIR(pair) " (" << pair.first << ", " << pair.second << ")"

bool checkFindOpTimingForValue(
	const TSOperationTimingVector& 		vWriters,
	const TSOperationTimingVector&		vParallels,
	size_t 								i,
	size_t								j,
	int									iValue,
	CheckReadersFlags&					oFlags
) {
	if (!(vParallels[j]->m_kOperationType & FIND))
		return true;
	
	auto pFindOperationTiming = static_cast<const FindOperationTiming*>(vParallels[j]);
	
	std::string s;
	s = pFindOperationTiming->m_tLockTime < vWriters[i]->m_tLockTime ? "<" : ">";
	
// 	std::cout 	<< "checkFindOpTimingForValue: " << COUT_PAIR(pFindOperationTiming->m_oResult) 
// 				<< ", ltr " << s << " ltw"
// 				<< ", i = " << i << ", j = " << j << ", v = " << iValue
// 	<< std::endl;
// 	
	if (
		(oFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult) 		&&
		pFindOperationTiming->m_tLockTime < vWriters[i]->m_tLockTime	&&
		pFindOperationTiming->m_oResult.second == iValue
	) {
// 		std::cout << "Find before FAIL" << COUT_PAIR(pFindOperationTiming->m_oResult) << std::endl;
		oFlags.m_kFlags ^= CheckReadersFlags::kReadBeforeResult;
	}
	
	if (vParallels[j]->m_tLockTime > vWriters[i]->m_tLockTime)
		oFlags.m_kFlags ^= CheckReadersFlags::kWriterHasNoReadersAfter;
	
	if (
		!(oFlags.m_kFlags & CheckReadersFlags::kReadAfterResult) 			&&
		!(oFlags.m_kFlags & CheckReadersFlags::kWriterHasNoReadersAfter)	&&
		pFindOperationTiming->m_oResult.second == iValue
	) {
// 		std::cout << "Find after " << COUT_PAIR(pFindOperationTiming->m_oResult) << ", v=" << iValue << std::endl;
		oFlags.m_kFlags |= CheckReadersFlags::kReadAfterResult;
	}
	if (oFlags.m_kFlags & CheckReadersFlags::kWriterHasNoReadersAfter)
// 		std::cout << "Writer Has No Readers after" << std::endl;

	return oFlags;
}

bool checkErOpTimingForValue(
	_IN const TSOperationTimingVector& 		vWriters,
	_IN const TSOperationTimingVector&		vParallels,
	_IN size_t 								i,
	_IN size_t								j,
	_IN int									iValue,
	_INOUT CheckReadersFlags&				oFlags
) {	
	EqualRangeType::const_iterator it, it_end;
	
	if (vParallels[j]->m_kOperationType & EQUAL_RANGE) 
	{
		auto pEqualRangeOperationTiming = static_cast<const EqualRangeOperationTiming*>(vParallels[j]);
		it = pEqualRangeOperationTiming->m_lResult.begin(),
		it_end = pEqualRangeOperationTiming->m_lResult.end();
	}
	else if (vParallels[j]->m_kOperationType & EQUAL_RANGE_Q) 
	{
		auto pEqualRangeQOperationTiming = static_cast<const EqualRangeQOperationTiming*>(vParallels[j]);
		it = pEqualRangeQOperationTiming->m_lResult.begin(),
		it_end = pEqualRangeQOperationTiming->m_lResult.end();
	} 
	else 
	{
		return true;
	}
		
	it = std::find_if(
		it, 
		it_end, 
		[iValue](TSTHashMapInt::ConstRefWeakPtrPairType pWeakPtrToPair) {
			return pWeakPtrToPair.lock()->second == iValue;
		}
	);
	
	if (
		(oFlags.m_kFlags & CheckReadersFlags::kReadBeforeResult)	&&
		vParallels[j]->m_tLockTime < vWriters[i]->m_tLockTime 		&&
		it != it_end
	) {
// 		std::cout << "Er Before FAIL" << COUT_PAIR((*(it->lock()))) << std::endl;
		oFlags.m_kFlags ^= CheckReadersFlags::kReadBeforeResult;
	}
	
	if (vParallels[j]->m_tLockTime > vWriters[i]->m_tLockTime)
		oFlags.m_kFlags ^= CheckReadersFlags::kWriterHasNoReadersAfter;
		
	if (
		!(oFlags.m_kFlags & CheckReadersFlags::kReadAfterResult) 			&&
		!(oFlags.m_kFlags & CheckReadersFlags::kWriterHasNoReadersAfter)	&&
		it != it_end
	) {
// 		std::cout << "Er After" << COUT_PAIR((*(it->lock()))) << std::endl;
		oFlags.m_kFlags |= CheckReadersFlags::kReadAfterResult;
	}
	
	return oFlags;
}

#undef TSTHashMapInt
#endif
