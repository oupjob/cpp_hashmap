#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <utility>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <memory>

#include <iostream>
#include <iomanip>

#include "hash_functions.h"

template <typename KeyT> struct HashMapKeyTypeTraits
{
	typedef KeyT*				PtrKeyType;
	typedef KeyT&				RefKeyType;
	typedef const KeyT*			ConstPtrKeyType;
	typedef const KeyT&			ConstRefKeyType;
	typedef ConstRefKeyType		HashFuncArgType;
};


template <> struct HashMapKeyTypeTraits<int>
{
	typedef int*				PtrKeyType;
	typedef int&				RefKeyType;
	typedef const int*			ConstPtrKeyType;
	typedef const int&			ConstRefKeyType;
	typedef int					HashFuncArgType;
};

// template <typename KeyT, typename T> struct PtrAllocTraits
// {
// 	typedef std::pair<KeyT, T> 		PairType;
// 	typedef PairType*				
// };
// 
// template <typename AllocTraits = PtrAllocTraits> class SharedPtrAllocator
// {
// 	
// }

#define DEFAULT_CAPACITY 127 

template <
	typename KeyT,
	typename T,
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<KeyT>::HashFuncArgType, size_t) = stdHashFunction
>
class HashMap 
{
public:
	typedef HashMapKeyTypeTraits<KeyT>				KeyTypeTraits;			
	typedef typename KeyTypeTraits::HashFuncArgType HashFuncArgType;
	
	typedef std::pair<KeyT, T> 						PairType;
	typedef PairType&								RefPairType;
	typedef const PairType&							ConstRefPairType;
	
	typedef T&										RefType;
	typedef const T&								ConstRefType;
	
	typedef std::shared_ptr<PairType>				SharedPtrPairType;
	typedef std::shared_ptr<const PairType>			ConstSharedPtrPairType;
	typedef std::weak_ptr<PairType>					WeakPtrPairType;
	typedef std::weak_ptr<const PairType>			ConstWeakPtrPairType;
	typedef const WeakPtrPairType&					ConstRefWeakPtrPairType;
	
	typedef std::list<WeakPtrPairType> 				EqualRangeType;
	typedef std::list<SharedPtrPairType>			RemovedRangeType;
	
	typedef std::shared_lock<std::shared_mutex>		ReadLockType;
	typedef std::lock_guard<std::shared_mutex>		WriteLockType;
	
	typedef std::shared_ptr<PairType>*				HashTableType;

	
protected:
	HashTableType				m_vHashTable;
	size_t						m_nCapacity;
	mutable std::shared_mutex	m_mSharedMutex;
	
	virtual hash_t firstIndex() const;
	virtual hash_t lastIndex() const;
	
	virtual inline hash_t findHashOfBusyCell(HashFuncArgType tKey, size_t iPos) const;
	virtual inline ConstRefPairType findImpl(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	virtual inline size_t equalRangeImpl(HashFuncArgType tKey, EqualRangeType& lOut) const;
	virtual inline size_t equalRangeImplForPos(HashFuncArgType tKey, size_t iPos, size_t nCount, EqualRangeType& lOut) const;
	
	virtual inline void insertImpl(HashFuncArgType tKey, ConstRefType tValue);
	virtual inline void removeImpl(HashFuncArgType tKey, size_t iPos, size_t nCount, RemovedRangeType& lOut);
	
	virtual inline void extendImpl(const size_t nNewCapacity);	
	virtual inline  bool emptyImpl() const;
	virtual inline  size_t sizeImpl() const;
	
	virtual inline  void clearImpl();
	
public:
	HashMap();
	HashMap(const std::initializer_list<PairType>& lInit);
	HashMap(const HashMap<KeyT, T, hashFunction>& oSrc);
	HashMap(HashMap<KeyT, T, hashFunction> && oSrc);
	virtual ~HashMap();
	
	virtual ConstRefPairType find(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	virtual EqualRangeType operator [] (HashFuncArgType tKey) const;
	virtual EqualRangeType equalRange(HashFuncArgType tKey, size_t iPos=0, size_t nCount=0) const;
	
	virtual void insert(HashFuncArgType tKey, ConstRefType tValue);
	virtual void insert(ConstRefPairType pair); 
	
	virtual RemovedRangeType remove(HashFuncArgType tKey, size_t iPos = 0, size_t nCount = 0);
	
	virtual void extend(const size_t nNewCapacity);
	
	virtual bool 	empty() const;
	virtual size_t	capacity() const;
	virtual size_t 	size() const;
	
	virtual void clear();
	
	virtual size_t defaultCapacity() const { return DEFAULT_CAPACITY; }
};

#define HM_TEMPLATE_DECL \
template <\
	typename KeyT,\
	typename T,\
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<KeyT>::HashFuncArgType, size_t)\
> 

#define THashMap HashMap<KeyT, T, hashFunction>

#define key(pair) pair.first
#define value(pair) pair.second

#define key_ptr(ptr) ptr->first
#define value_ptr(ptr) ptr->second

#define cell_is_free(cell) !cell
#define cell_is_busy(cell) cell

#define is_valid_hash(h) h < m_nCapacity
#define is_invalid_hash(h) h >= m_nCapacity

HM_TEMPLATE_DECL THashMap::HashMap() 
{
	m_nCapacity = DEFAULT_CAPACITY;	
	
	m_vHashTable = new SharedPtrPairType[m_nCapacity];
	std::memset(m_vHashTable, 0, sizeof(SharedPtrPairType) * m_nCapacity);
}

HM_TEMPLATE_DECL THashMap::HashMap(const std::initializer_list<THashMap::PairType>& lInit) 
{
	m_nCapacity = DEFAULT_CAPACITY;	
	
	typedef std::initializer_list<PairType> InitList;
	typename InitList::iterator it_l = lInit.begin(), it_end = lInit.end();
	
	size_t iIterDistance = std::distance(it_l, it_end);
	m_nCapacity = (iIterDistance > m_nCapacity) ? iIterDistance + DEFAULT_CAPACITY : m_nCapacity;
	
	m_vHashTable = new SharedPtrPairType[m_nCapacity];
	std::memset(m_vHashTable, 0, sizeof(SharedPtrPairType) * m_nCapacity);
	
	for(; it_l != it_end; ++it_l)
	{
		insert(*it_l);
	}
}

HM_TEMPLATE_DECL THashMap::HashMap(const THashMap& oSrc) 
{	
	m_nCapacity = oSrc.m_nCapacity;	
	m_vHashTable = new SharedPtrPairType[m_nCapacity];
	
	for(size_t i = 0; i < m_nCapacity; ++i)
	{
		if (oSrc.m_vHashTable[i])
			m_vHashTable[i] = oSrc.m_vHashTable[i];
		else
			m_vHashTable[i] = nullptr;
	}
}

HM_TEMPLATE_DECL THashMap::HashMap(THashMap && oSrc) 
{	
	m_nCapacity = oSrc.m_nCapacity;	
	m_vHashTable = oSrc.m_vHashTable;
	
	oSrc.m_vHashTable = nullptr;
	oSrc.m_nCapacity = 0;
}

HM_TEMPLATE_DECL THashMap::~HashMap()
{
	delete [] m_vHashTable;
}

HM_TEMPLATE_DECL 
hash_t THashMap::findHashOfBusyCell(const typename THashMap::HashFuncArgType tKey, size_t iPos) const
{	
	PairType* pLast = nullptr;
	size_t iHits = 0;
	hash_t h;
			
	if (iPos >= m_nCapacity) return m_nCapacity;
	++iPos;	
	
	for(size_t ik = 0; ik < m_nCapacity; ++ik)
	{
		h = hashFunction(m_nCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		++iHits;
		
		if (iHits == iPos) {
			return h;
		}
	}
	
	return m_nCapacity;
}

HM_TEMPLATE_DECL
typename THashMap::ConstRefPairType 
THashMap::findImpl(THashMap::HashFuncArgType tKey, THashMap::ConstRefPairType oDefaultPair, size_t iPos) const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
		
	hash_t h = findHashOfBusyCell(tKey, iPos);
	if (is_valid_hash(h) && key_ptr(m_vHashTable[h]) == tKey) {
		return *m_vHashTable[h];
	} else {
		return oDefaultPair;
	}
}

HM_TEMPLATE_DECL
typename THashMap::ConstRefPairType 
THashMap::find(THashMap::HashFuncArgType tKey, THashMap::ConstRefPairType oDefaultPair, size_t iPos) const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
	
	return findImpl(tKey, oDefaultPair, iPos);
}

HM_TEMPLATE_DECL
size_t THashMap::equalRangeImpl(typename THashMap::HashFuncArgType tKey, typename THashMap::EqualRangeType& lOut) const 
{	
	size_t ik = 0;
	for(; ik < m_nCapacity; ++ik) {
		hash_t h = hashFunction(m_nCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		lOut.push_back(m_vHashTable[h]);
	}
	
	return ik;
}

HM_TEMPLATE_DECL
size_t THashMap::equalRangeImplForPos(
	typename THashMap::HashFuncArgType 		tKey, 
	size_t									iPos,
	size_t									nCount,
	typename THashMap::EqualRangeType& 		lOut
) const 
{	
	++iPos;
	size_t ik = 0, iHits = 0;
	for(; ik < m_nCapacity; ++ik) {
		hash_t h = hashFunction(m_nCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		++iHits;
		
		if (iHits < iPos) {
			continue;
		} else if (nCount && iHits - iPos == nCount) {
			return ik;
		}
		
		lOut.push_back(m_vHashTable[h]);
	}
	
	return ik;
}

HM_TEMPLATE_DECL
typename THashMap::EqualRangeType 
THashMap::equalRange(const typename THashMap::HashFuncArgType tKey, size_t iPos, size_t nCount) const 
{	
	ReadLockType	lReadLock(m_mSharedMutex);
	EqualRangeType 	lEqualRange;
	
	if (!iPos && !nCount) {
		equalRangeImpl(tKey, lEqualRange);
	} else {
		equalRangeImplForPos(tKey, iPos, nCount, lEqualRange);
	}
		
	return std::move(lEqualRange);
}

HM_TEMPLATE_DECL 
typename THashMap::EqualRangeType 
THashMap::operator [] (const typename THashMap::HashFuncArgType tKey) const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
	EqualRangeType 	lEqualRange;
	
	equalRangeImpl(tKey, lEqualRange);
	return std::move(lEqualRange);
}

HM_TEMPLATE_DECL  
void THashMap::insertImpl(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefType tValue)
{
	hash_t h;
	while(true) 
	{
		for(size_t ik = 0; ik < m_nCapacity; ++ik) 
		{
			h = hashFunction(m_nCapacity, tKey, ik);
			if (cell_is_free(m_vHashTable[h])) {
				m_vHashTable[h] = std::make_shared<PairType>(PairType(tKey, tValue));
				return;
			}
		}
		
		extendImpl(m_nCapacity + DEFAULT_CAPACITY);
	}
}

HM_TEMPLATE_DECL  
void THashMap::insert(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefType tValue)
{
	WriteLockType	lWriteLock(m_mSharedMutex);
	
	insertImpl(tKey, tValue);
}

HM_TEMPLATE_DECL
void THashMap::insert(typename THashMap::ConstRefPairType pair) 
{
	insert(key(pair), value(pair));
}

HM_TEMPLATE_DECL 
void THashMap::removeImpl(	
	const typename THashMap::HashFuncArgType 	tKey,
	size_t 										iPos,
	size_t										nCount,
	THashMap::RemovedRangeType&					lOut
) {
	++iPos;
	if (!nCount) nCount = m_nCapacity - 1;
	size_t ik = 0, iHits = 0;
	for(; ik < m_nCapacity; ++ik) {
		hash_t h = hashFunction(m_nCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		if (++iHits < iPos) {
			continue;
		} else if (iHits - iPos < nCount) {
			lOut.push_back(m_vHashTable[h]);
			m_vHashTable[h] = nullptr;
		}
	}
} 

HM_TEMPLATE_DECL
typename THashMap::RemovedRangeType 
THashMap::remove(
	const typename THashMap::HashFuncArgType 	tKey,
	size_t 										iPos,
	size_t										nCount
) {
	WriteLockType		lWriteLock(m_mSharedMutex);
	RemovedRangeType	lRemovedRange;
	
	removeImpl(tKey, iPos, nCount, lRemovedRange);
	
	return lRemovedRange;
}

// this realization don't use insert, because insert used KeyT and T copy constructors
// so, we just copy pointers to pairs to new hash table
HM_TEMPLATE_DECL
void THashMap::extendImpl(const size_t nNewCapacity)
{	
	if (nNewCapacity <= m_nCapacity)
		return;
		
	HashTableType vNewHashTable = new SharedPtrPairType[nNewCapacity];
	std::memset(vNewHashTable, 0, sizeof(SharedPtrPairType) * nNewCapacity);
	
	size_t nSize = sizeImpl();
	
	while(true)
	{
		size_t iIndex = firstIndex();
		if (iIndex == m_nCapacity)
			break;
		
		KeyT tKey = key_ptr(m_vHashTable[iIndex]);
		
		for(size_t ik = 0; ik < m_nCapacity; ++ik)
		{
			iIndex = hashFunction(m_nCapacity, tKey, ik);
			if (cell_is_busy(m_vHashTable[iIndex]) && key_ptr(m_vHashTable[iIndex]) == tKey) 
			{
				for(size_t ik_new = 0; ik_new < nNewCapacity; ++ik_new) {
					size_t iNewIndex = hashFunction(nNewCapacity, tKey, ik_new);
					if (cell_is_free(vNewHashTable[iNewIndex])) {
						vNewHashTable[iNewIndex] = m_vHashTable[iIndex];
						m_vHashTable[iIndex] = nullptr;						
						break;
					}
				}
			}
		}
	}
	
	delete [] m_vHashTable;
	
	m_vHashTable = vNewHashTable;
	m_nCapacity = nNewCapacity;	
}

HM_TEMPLATE_DECL
void THashMap::extend(const size_t nNewCapacity)
{
	WriteLockType	lWriteLock(m_mSharedMutex);
	extendImpl(nNewCapacity);
}

HM_TEMPLATE_DECL
size_t THashMap::sizeImpl() const {
	size_t n = 0;
	for(size_t m = 0; m < m_nCapacity; ++m)
		n += static_cast<bool>(cell_is_busy(m_vHashTable[m]));

	return n;
}

HM_TEMPLATE_DECL
size_t THashMap::size() const {
	ReadLockType lReadLock(m_mSharedMutex);
	
	return sizeImpl();
}

HM_TEMPLATE_DECL
bool THashMap::emptyImpl() const {
	return firstIndex() >= m_nCapacity;
}

HM_TEMPLATE_DECL
bool THashMap::empty() const {
	ReadLockType lReadLock(m_mSharedMutex);
	
	return emptyImpl();
}

HM_TEMPLATE_DECL
size_t THashMap::capacity() const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
	
	return m_nCapacity;
}

HM_TEMPLATE_DECL
hash_t THashMap::firstIndex() const {
	for(hash_t i = 0; i < m_nCapacity; ++i) {
		if(cell_is_busy(m_vHashTable[i]))
			return i;
	}
	
	return m_nCapacity;
}

HM_TEMPLATE_DECL
hash_t THashMap::lastIndex() const {	
	for(hash_t i = m_nCapacity - 1; i >= 0; --i) {
		if(cell_is_busy( m_vHashTable[i]))
			return i;
	}
	
	return m_nCapacity;
}

HM_TEMPLATE_DECL void THashMap::clearImpl()
{
	delete [] m_vHashTable;
	m_nCapacity = DEFAULT_CAPACITY;
	
	m_vHashTable = new SharedPtrPairType[m_nCapacity];
}

HM_TEMPLATE_DECL void THashMap::clear()
{
	WriteLockType lWriteLock(m_mSharedMutex);
	
	clearImpl();
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
