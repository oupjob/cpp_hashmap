#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <utility>
#include <cstring>
#include <mutex>
#include <cmath>

#include <iostream>

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

#define DEFAULT_CAPACITY 128 

template <
	typename KeyT,
	typename T,
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<KeyT>::HashFuncArgType, size_t) = stdHashFunction
>
class HashMap 
{
public:
	typedef T*										PtrType;
	typedef T&										RefType;
	
	typedef HashMapKeyTypeTraits<KeyT>				KeyTypeTraits;			
	typedef typename KeyTypeTraits::PtrKeyType		PtrKeyType;
	typedef typename KeyTypeTraits::RefKeyType		RefKeyType;
	typedef typename KeyTypeTraits::HashFuncArgType HashFuncArgType;
	
	typedef std::pair<KeyT, T> 						PairType;
	typedef PairType&								RefPairType;
	typedef PairType*								PtrPairType;
	typedef std::list<PairType> 					EqualRangeType;
	
	typedef std::lock_guard<std::recursive_mutex>	LockGuardType;

	typedef typename KeyTypeTraits::ConstPtrKeyType	ConstPtrKeyType;
	typedef typename KeyTypeTraits::ConstRefKeyType 	ConstRefKeyType;
	typedef const T*								ConstPtrType;
	typedef const T&								ConstRefType;
	
	typedef const PairType&							ConstRefPairType;
	typedef const PairType&							ConstPtrPairType;
	
	typedef PairType**								HashTableType;
	
// 	std::pair<hash_t, hash_t> 	IndexPairType;
	
private:
	HashTableType			m_vHashTable;
	size_t					m_iCapacity;
	size_t					m_iSize;
	std::recursive_mutex	m_mRecursiveMutex;
	
	void rehash();
	hash_t findHashOfBusyCell(HashFuncArgType tKey, size_t iPos = 0) const;
	
	void equalRangeImpl(HashFuncArgType tKey, EqualRangeType& lOut) const;
	
	inline int	gcd(int a, int b);
	inline int 	minCoprimeWithCapacity();
	
public:
	HashMap();
	HashMap(const std::initializer_list<PairType>& lInit);
	HashMap(const HashMap<KeyT, T, hashFunction>& src);
	~HashMap();
	
	ConstRefPairType find(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	EqualRangeType operator [] (HashFuncArgType tKey) const;
	EqualRangeType equalRange(HashFuncArgType tKey) const;
	
	void insert(HashFuncArgType tKey, ConstRefType tValue);
	void insert(ConstRefPairType pair); 
	
	PairType remove(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0);
	
	void extend(const size_t iNewCapacity);
	
	bool 		empty() const { return !m_iSize; } 
	size_t 		size() const { return m_iSize; }
	size_t		capacity() const { return m_iCapacity; }
	bool		isDefaultPair(RefPairType oPair, RefPairType oDefaultPair);
	
	void clear();
};

#define HM_TEMPLATE_DEF \
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

#define is_valid_hash(h) h < m_iCapacity
#define is_invalid_hash(h) h >= m_iCapacity

HM_TEMPLATE_DEF THashMap::HashMap() 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	m_iSize = 0;
	
	m_vHashTable = new PtrPairType[m_iCapacity];
	std::memset(m_vHashTable, 0, sizeof(PtrPairType) * m_iCapacity);
}

HM_TEMPLATE_DEF THashMap::HashMap(const std::initializer_list<THashMap::PairType>& lInit) 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	m_iSize = 0;
	
	typedef std::initializer_list<PairType> InitList;
	typename InitList::iterator it_l = lInit.begin(), it_end = lInit.end();
	
	size_t iIterDistance = std::distance(it_l, it_end);
	m_iCapacity = (iIterDistance > m_iCapacity) ? iIterDistance + DEFAULT_CAPACITY : m_iCapacity;
	
	m_vHashTable = new PairType*[m_iCapacity];
	std::memset(m_vHashTable, 0, sizeof(PtrPairType) * m_iCapacity);
	
	for(; it_l != it_end; ++it_l)
	{
		insert(*it_l);
	}
}

HM_TEMPLATE_DEF THashMap::HashMap(const THashMap& src) 
{	
	m_iCapacity = src.m_iCapacity;
	m_iSize = src.m_iSize;
	
	m_vHashTable = new PairType*[m_iCapacity];
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (src.m_vHashTable[i])
			m_vHashTable[i] = new PairType((*src.m_vHashTable[i]));
		else
			m_vHashTable[i] = nullptr;
	}
}

HM_TEMPLATE_DEF THashMap::~HashMap()
{
	LockGuardType oLock(m_mRecursiveMutex);
	clear();
	delete [] m_vHashTable;
}

HM_TEMPLATE_DEF
bool THashMap::isDefaultPair(RefPairType oPair, RefPairType oDefaultPair) 
{
	return key(oPair) == key(oDefaultPair) && value(oPair) == value(oDefaultPair); 
}

HM_TEMPLATE_DEF 
hash_t THashMap::findHashOfBusyCell(const typename THashMap::HashFuncArgType tKey, size_t iPos) const
{
	LockGuardType oLock(const_cast<std::recursive_mutex&>(m_mRecursiveMutex));
	
	PairType* pLast = nullptr;
	size_t iHits = 0;
	++iPos;
	size_t ik = 0;
	hash_t h;
	
	if (iPos >= m_iSize + 1) return m_iCapacity;
		
	while(true) {
		// ik == 1
		// h(x) = (ax + b) mod p => (h(x) + ik+m) mod m = (h(x) + ik) mod m 
		h = hashFunction(m_iCapacity, tKey, ik);
		pLast = m_vHashTable[h];
		
		if (ik >= m_iSize)
			return m_iCapacity;
		
		if (pLast) { 
			if (key((*pLast)) == tKey) {
				++iHits;
				if (iHits > iPos) {
					return m_iCapacity;
				} else if (iHits == iPos) {
					return h;
				} else {
					++ik;
					continue;
				}
			} else {
				++ik;
				continue;
			}
		} else {
			return m_iCapacity;
		}
	}
}

HM_TEMPLATE_DEF
typename THashMap::ConstRefPairType 
THashMap::find(
	THashMap::HashFuncArgType tKey, THashMap::ConstRefPairType oDefault, size_t iPos
) const 
{
	hash_t h = findHashOfBusyCell(tKey, iPos);
	if (is_valid_hash(h) && key_ptr(m_vHashTable[h]) == tKey) {
		return *m_vHashTable[h];
	} else {
		return oDefault;
	}
}

HM_TEMPLATE_DEF
void THashMap::equalRangeImpl(
	const typename THashMap::HashFuncArgType tKey, 
	typename THashMap::EqualRangeType& lOut
) const {
	LockGuardType oLock(const_cast<std::recursive_mutex&>(m_mRecursiveMutex));
	
	size_t iPos = 0;
	
	while(true) {
		hash_t h = findHashOfBusyCell(tKey, iPos);
		if (is_invalid_hash(h) || key_ptr(m_vHashTable[h]) != tKey)
			return;
		
		lOut.push_back(*m_vHashTable[h]);
		++iPos;
	}
}

HM_TEMPLATE_DEF
typename THashMap::EqualRangeType 
THashMap::equalRange(const typename THashMap::HashFuncArgType tKey) const 
{	
	EqualRangeType lEqualRange;
	equalRangeImpl(tKey, lEqualRange);
	return lEqualRange;
}

HM_TEMPLATE_DEF 
typename THashMap::EqualRangeType 
THashMap::operator [] (const typename THashMap::HashFuncArgType tKey) const 
{	
	EqualRangeType lEqualRange;
	equalRangeImpl(tKey, lEqualRange);
	return lEqualRange;
}

HM_TEMPLATE_DEF  
void THashMap::insert(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefType tValue)
{
	LockGuardType oLock(m_mRecursiveMutex);
	hash_t h;
	
	while(true) 
	{
		PtrPairType * pLast = nullptr;
		for(size_t ik = 0; ik <= m_iSize || !m_iSize; ++ik) 
		{
			h = hashFunction(m_iCapacity, tKey, ik);
			if (!m_vHashTable[h]) {
				m_vHashTable[h] = new PairType(tKey, tValue);
				++m_iSize;
				return;
			}
		}
		
		extend(m_iCapacity + DEFAULT_CAPACITY);
	}
}

HM_TEMPLATE_DEF
void THashMap::insert(typename THashMap::ConstRefPairType pair) 
{
	insert(key(pair), value(pair));
}

HM_TEMPLATE_DEF
typename THashMap::PairType 
THashMap::remove(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefPairType oDefaultPair, size_t iPos)
{
	LockGuardType oLock(m_mRecursiveMutex);
	
	hash_t h = findHashOfBusyCell(tKey, iPos);
	if (is_valid_hash(h)) {
		PairType oPair(*m_vHashTable[h]);
		delete m_vHashTable[h];
		m_vHashTable[h] = nullptr;
		--m_iSize;
		return oPair;
	}
	
	return oDefaultPair;
}
// this realization don't use insert, because insert used KeyT and T copy constructors
// so, we just copy pointers to pairs to new hash table
// REM: m_iSize will not changed after this operation
HM_TEMPLATE_DEF
void THashMap::extend(const size_t iNewCapacity)
{
	LockGuardType oLock(m_mRecursiveMutex);
	
	if (iNewCapacity <= m_iCapacity)
		return;
		
	HashTableType vNewHashTable = new PtrPairType[iNewCapacity];
	std::memset(vNewHashTable, 0, sizeof(PtrPairType) * iNewCapacity);
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (cell_is_free(m_vHashTable[i]))
			continue;
		
		size_t ik = 0;
		
		do {
			hash_t iNewHash = hashFunction(iNewCapacity, key((*m_vHashTable[i])), ik);
			if (cell_is_free(vNewHashTable[iNewHash])) {
				vNewHashTable[iNewHash] = m_vHashTable[i];
				break;
			}
			++ik;
		} while(true);
	}
	
	delete [] m_vHashTable;
	m_vHashTable = vNewHashTable;
	m_iCapacity = iNewCapacity;	
}

HM_TEMPLATE_DEF void THashMap::clear()
{
	LockGuardType oLock(m_mRecursiveMutex);
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (m_vHashTable[i]) {
			delete m_vHashTable[i];
			m_vHashTable[i] = 0;
		}
	}
	m_vHashTable = nullptr;
	m_iSize = m_iCapacity = 0;
}

#undef key
#undef value
#undef ptr
#undef hash
#undef cell_is_free
#undef cell_is_busy
#undef HM_TEMPLATE_DEF
#undef THashMap
#undef DEFAULT_CAPACITY

#endif
