#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <utility>
#include <cstring>
#include <mutex>

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
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<KeyT>::HashFuncArgType, int) = stdHashFunction
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
	typedef std::list<const RefPairType> 			EqualRangeType;
	typedef std::pair<PtrPairType, hash_t>			PtrToHashPairType;
	
	typedef std::lock_guard<std::recursive_mutex>	LockGuardType;

	typedef typename KeyTypeTraits::ConstPtrKeyType	ConstPtrKeyType;
	typedef typename KeyTypeTraits::ConstRefKeyType ConstRefKeyType;
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
	static PairType			m_oDefaultPair;
	std::recursive_mutex	m_mRecursiveMutex;
	
	void extend(const size_t iNewCapacity);
	void rehash();
	PtrToHashPairType findPtr(HashFuncArgType tKey, size_t iPos = 0) const;
	
	void equalRangeImpl(HashFuncArgType tKey, EqualRangeType& lOut) const;
	
	inline int	gcd(int a, int b);
	inline int 	minCoprimeWithCapacity();
	
public:
	HashMap();
	HashMap(std::initializer_list<PairType>& lInit);
	HashMap(const HashMap<KeyT, T, hashFunction>& src);
	~HashMap();
	
	ConstRefPairType defaultPair() const;
	RefPairType find(HashFuncArgType tKey, size_t iPos = 0, ConstRefPairType oDefault = m_oDefaultPair) const;
	
	EqualRangeType operator [] (HashFuncArgType tKey) const;
	EqualRangeType equalRange(HashFuncArgType tKey) const;
	
	void insert(HashFuncArgType tKey, ConstRefType tValue);
	void insert(ConstRefPairType pair); 
	
	PairType remove(HashFuncArgType tKey, size_t iPos);
	
	bool 		empty() const { return !m_iSize; } 
	size_t 		size() const { return m_iSize; }
	size_t		capacity() const { return m_iCapacity; }
	bool		isDefaultPair(RefPairType oPair);
	
	void clear();
};

#define HM_TEMPLATE_DEF \
template <\
	typename KeyT,\
	typename T,\
	hash_t(hashFunction)(size_t, typename HashMapKeyTypeTraits<KeyT>::HashFuncArgType, int)\
> 

#define THashMap HashMap<KeyT, T, hashFunction>

#define key(pair) pair.first
#define value(pair) pair.second

#define ptr(pair) pair.first
#define hash(pair) pair.second

#define cell_is_free(cell) !cell
#define cell_is_busy(cell) cell

HM_TEMPLATE_DEF THashMap::HashMap() 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	m_oDefaultPair = PairType(KeyT(), T());
	m_iSize = 0;
	
	m_vHashTable = new PtrPairType[m_iCapacity];
	std::memset(m_vHashTable, nullptr, sizeof(PtrPairType) * m_iCapacity);
}

HM_TEMPLATE_DEF THashMap::HashMap(std::initializer_list<THashMap::PairType>& lInit) 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	m_oDefaultPair = PairType(KeyT(), T());
	m_iSize = 0;
	
	typedef std::initializer_list<PairType> InitList;
	typename InitList::iterator it_l = lInit.begin(), it_end = lInit.end();
	
	auto iIterDistance = std::distance(it_l, it_end);
	m_iCapacity = (iIterDistance > m_iCapacity) ? iIterDistance + DEFAULT_CAPACITY : m_iCapacity;
		
	m_vHashTable = new PtrPairType[m_iCapacity];
	std::memset(m_vHashTable, nullptr, sizeof(PtrPairType) * m_iCapacity);
	
	for(; it_l != it_end; ++it_l)
	{
		insert(*it_l);
	}
}

HM_TEMPLATE_DEF THashMap::HashMap(const THashMap& src) 
{	
	m_iCapacity = src.m_iCapacity;
	m_oDefaultPair = src.m_oDefaultPair;
	
	if (m_vHashTable) {
		clear();
		delete[] m_vHashTable;
	}
	m_vHashTable = new PairType[m_iCapacity];
	
	for(int i = 0; i < m_iCapacity; ++i)
	{
		if (src.m_vHashTable[i])
			m_vHashTable[i] = new PairType(src.m_vHashTable[i]);
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
bool THashMap::isDefaultPair(RefPairType oPair) 
{
	return key(oPair) == key(m_oDefaultPair) && value(oPair) == value(m_oDefaultPair); 
}

HM_TEMPLATE_DEF 
typename THashMap::PtrToHashPairType 
THashMap::findPtr(const typename THashMap::HashFuncArgType tKey, size_t iPos) const
{
	LockGuardType oLock(m_mRecursiveMutex);
	
	PairType* pLast = nullptr;
	size_t iHits = 0;
	++iPos;
	int ik = 0;
	hash_t h;
	
	if (iPos >= m_iSize) return 0;
	
	while(true) {
		// ik == 1
		// h(x) = (ax + b) mod p => (h(x) + ik+m) mod m = (h(x) + ik) mod m 
		if (ik >= m_iSize)
			return PtrToHashPairType(nullptr, h);
		
		h = hashFunction(m_iCapacity, tKey, ik);
		pLast = m_vHashTable[h];
		
		if (pLast) { 
			if (key(*pLast) == tKey) {
				++iHits;
				if (iHits == iPos) {
					return pLast;
				} else {
					++ik;
					continue;
				}
			} else {
				++ik;
				continue;
			}
		} else {
			return (nullptr, h);
		}
	}
}

HM_TEMPLATE_DEF
typename THashMap::RefPairType 
THashMap::find(
	THashMap::HashFuncArgType tKey, size_t iPos, THashMap::ConstRefPairType oDefault
) const 
{
	const PtrToHashPairType oPair = findPtr(tKey, iPos);
	if (ptr(oPair)) {
		return *(ptr(oPair));
	} else {
		return oDefault;
	}
}

HM_TEMPLATE_DEF
void THashMap::equalRangeImpl(
	const typename THashMap::HashFuncArgType tKey, 
	typename THashMap::EqualRangeType& lOut
) const {
	LockGuardType oLock(m_mRecursiveMutex);
	
	size_t iPos = 0;
	
	while(true) {
		PtrToHashPairType oPtrToHash = findPtr(tKey, iPos);
		if (!ptr(oPtrToHash))
			return;
		
		lOut.push_back(*(ptr(oPtrToHash)));
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
	
	if (m_iSize + 1 == m_iCapacity) {
		extend(m_iCapacity + DEFAULT_CAPACITY);
	}
	
	for(size_t iPos = 0; ; ++iPos) 
	{ 
		PtrToHashPairType oPtrToHash = findPtr(tKey, iPos, m_oDefaultPair);
		if (!ptr(oPtrToHash)) {
			m_vHashTable[hash(oPtrToHash)] = new PairType(tKey, tValue);
			++m_iSize;
			return;
		}
	}
}

HM_TEMPLATE_DEF
void THashMap::insert(typename THashMap::ConstRefPairType pair) 
{
	insert(key(pair), value(pair));
}

HM_TEMPLATE_DEF
typename THashMap::PairType 
THashMap::remove(const typename THashMap::HashFuncArgType tKey, size_t iPos)
{
	LockGuardType oLock(m_mRecursiveMutex);
	
	PtrToHashPairType oPtrToHash = findPtr(tKey, iPos, m_oDefaultPair);
	if (ptr(oPtrToHash)) {
		PtrType oPair(*ptr(oPtrToHash));
		delete m_vHashTable[hash(oPtrToHash)];
		m_vHashTable[hash(oPtrToHash)] = nullptr;
		--m_iSize;
		return oPair;
	}
	
	return m_oDefaultPair;
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
		
	HashTableType vNewHashTable = new PairType[iNewCapacity];
	std::memset(vNewHashTable, nullptr, sizeof(PtrPairType) * iNewCapacity);
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (cell_is_free(m_vHashTable[i]))
			continue;
		
		int ik = 0;
		
		do {
			hash_t iNewHash = hashFunction(iNewCapacity, key(*m_vHashTable[i]), ik);
			if (cell_is_free(vNewHashTable[iNewHash])) {
				vNewHashTable[iNewHash] = m_vHashTable[i];
				break;
			}
			++ik;
		} while(true);
	}
	
	clear();
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
