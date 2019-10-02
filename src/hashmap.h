#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <utility>
#include <cstring>
#include <mutex>
#include <cmath>

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

#define DEFAULT_CAPACITY 127 

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
	
	typedef std::lock_guard<std::recursive_mutex>	LockGuardType;

	typedef typename KeyTypeTraits::ConstPtrKeyType	ConstPtrKeyType;
	typedef typename KeyTypeTraits::ConstRefKeyType ConstRefKeyType;
	typedef const T*								ConstPtrType;
	typedef const T&								ConstRefType;
	
	typedef const PairType&							ConstRefPairType;
	typedef const PairType*							ConstPtrPairType;
	
	typedef std::list<ConstPtrPairType> 			EqualRangeType;
	typedef std::list<hash_t>						EqualRangeIndexListType;
	
	typedef std::list<PtrPairType>					DanglingChainRangeType;
	typedef PairType**								HashTableType;
	
// 	std::pair<hash_t, hash_t> 	IndexPairType;
	
private:
	HashTableType			m_vHashTable;
	size_t					m_iCapacity;
	size_t					m_iSize;
	std::recursive_mutex	m_mRecursiveMutex;
	
	hash_t firstIndex();
	hash_t lastIndex();
	
	hash_t findHashOfBusyCell(HashFuncArgType tKey, size_t iPos = 0) const;
	
	size_t equalRangeImpl(HashFuncArgType tKey, EqualRangeType& lOut) const;
	size_t equalRangeImplForPos(HashFuncArgType tKey, size_t iPos, size_t nCount, EqualRangeType& lOut) const;	
	
	void buildDanglingChain(DanglingChainRangeType& out);
	
public:
	HashMap();
	HashMap(const std::initializer_list<PairType>& lInit);
	HashMap(const HashMap<KeyT, T, hashFunction>& oSrc);
	~HashMap();
	
	ConstRefPairType find(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	EqualRangeType operator [] (HashFuncArgType tKey) const;
	EqualRangeType equalRange(HashFuncArgType tKey, size_t iPos=0, size_t nCount=0) const;
	
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

HM_TEMPLATE_DEF THashMap::HashMap(const THashMap& oSrc) 
{	
	m_iCapacity = oSrc.m_iCapacity;
	m_iSize = oSrc.m_iSize;
	
	m_vHashTable = new PairType*[m_iCapacity];
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (oSrc.m_vHashTable[i])
			m_vHashTable[i] = new PairType((*oSrc.m_vHashTable[i]));
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
	hash_t h;
			
	if (iPos >= m_iSize) return m_iCapacity;
	++iPos;	
	
	for(size_t ik = 0; ik < m_iCapacity; ++ik)
	{
		h = hashFunction(m_iCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		++iHits;
		
		if (iHits == iPos) {
			return h;
		}
	}
	
	return m_iCapacity;
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
size_t THashMap::equalRangeImpl(typename THashMap::HashFuncArgType tKey, typename THashMap::EqualRangeType& lOut) const 
{
	LockGuardType oLock(const_cast<std::recursive_mutex&>(m_mRecursiveMutex));
	
	size_t ik = 0;
	for(; ik < m_iCapacity; ++ik) {
		hash_t h = hashFunction(m_iCapacity, tKey, ik);
		if (cell_is_free(m_vHashTable[h]) || key_ptr(m_vHashTable[h]) != tKey)
			continue;
		
		lOut.push_back(m_vHashTable[h]);
	}
	
	return ik;
}

HM_TEMPLATE_DEF
size_t THashMap::equalRangeImplForPos(
	typename THashMap::HashFuncArgType 		tKey, 
	size_t									iPos,
	size_t									nCount,
	typename THashMap::EqualRangeType& 		lOut
) const 
{
	LockGuardType oLock(const_cast<std::recursive_mutex&>(m_mRecursiveMutex));
	
	++iPos;
	size_t ik = 0, iHits = 0;
	for(; ik < m_iCapacity; ++ik) {
		hash_t h = hashFunction(m_iCapacity, tKey, ik);
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

HM_TEMPLATE_DEF
typename THashMap::EqualRangeType 
THashMap::equalRange(const typename THashMap::HashFuncArgType tKey, size_t iPos, size_t nCount) const 
{	
	EqualRangeType lEqualRange;
	
	if (!iPos && !nCount) {
		equalRangeImpl(tKey, lEqualRange);
	} else {
		equalRangeImplForPos(tKey, iPos, nCount, lEqualRange);
	}
		
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
	
// 	std::cout << "insert(key=" << tKey << ", value=" << tValue << ")" << std::endl;
	
	while(true) 
	{
		for(size_t ik = 0; ik < m_iCapacity; ++ik) 
		{
			h = hashFunction(m_iCapacity, tKey, ik);
			if (cell_is_free(m_vHashTable[h])) {
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
		
		// move last equal pairs to its `iPos - 1`	
		hash_t h_prev = h;
		while(true) {
			hash_t h_cur = findHashOfBusyCell(tKey, iPos);
			if (is_invalid_hash(h_cur))
				return oPair;
			
			m_vHashTable[h_prev] = m_vHashTable[h_cur];
			m_vHashTable[h_cur] = nullptr;
			
			h_prev = h_cur;
			++iPos;
		}
		
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
	
	DanglingChainRangeType lDanglingChain;
	buildDanglingChain(lDanglingChain);
	
// 	dump("EXTEND: before");
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (cell_is_free(m_vHashTable[i]))
			continue;
		
		size_t ik = 0;
		do {
			hash_t iNewHash = hashFunction(iNewCapacity, key_ptr(m_vHashTable[i]), ik);
			if (cell_is_free(vNewHashTable[iNewHash])) {
				vNewHashTable[iNewHash] = m_vHashTable[i];
				break;
			}
			++ik;
		} while(true);
	}
	
	typename DanglingChainRangeType::iterator it_l = lDanglingChain.begin(), it_end = lDanglingChain.end();
	for(; it_l != it_end; ++it_l) {
		size_t ik = 0;
		do {
			hash_t iNewHash = hashFunction(iNewCapacity, key_ptr((*it_l)), ik);
			if (cell_is_free(vNewHashTable[iNewHash])) {
				vNewHashTable[iNewHash] = *it_l;
				break;
			}
			++ik;
		} while(true);
	}
	
//  dump("EXTEND: after");
	delete [] m_vHashTable;
	m_vHashTable = vNewHashTable;
	m_iCapacity = iNewCapacity;	
}

#define move_last(l_out, ptr) \
l_out.push_front(ptr);\
ptr = nullptr;

#define move_first(l_out, ptr) \
l_out.push_back(ptr);\
ptr = nullptr;

HM_TEMPLATE_DEF
void THashMap::buildDanglingChain(THashMap::DanglingChainRangeType& lOut)
{
	hash_t 	iFirst = firstIndex(),
			iLast = lastIndex();
			
	if (iFirst == iLast || iFirst == m_iCapacity || iLast == m_iCapacity)
		return;
	
	hash_t 	hFirst = hashFunction(m_iCapacity, key_ptr(m_vHashTable[iFirst]), 0),
			hLast = hashFunction(m_iCapacity, key_ptr(m_vHashTable[iFirst]), 0);
			
	if (hFirst != hLast)
		return;
	
	move_last(lOut, m_vHashTable[iLast])
	for(hash_t i = iLast - 1; i != iFirst; --i)
	{
		if (!m_vHashTable[i] || hashFunction(m_iCapacity, key_ptr(m_vHashTable[i]), 0) != hLast)
			break;
		
		move_last(lOut, m_vHashTable[i])
	}
	
	move_first(lOut, m_vHashTable[iFirst])
	for(hash_t i = iFirst + 1; i != iLast; ++i)
	{
		if (!m_vHashTable[i] || hashFunction(m_iCapacity, key_ptr(m_vHashTable[i]), 0) != hFirst)
			break;
		
		move_first(lOut, m_vHashTable[i])
	}
}

#undef move_first
#undef move_last

HM_TEMPLATE_DEF
hash_t THashMap::firstIndex() {
	hash_t i = 0;
	for(; i < m_iCapacity; ++i) {
		if(m_vHashTable[i])
			return i;
	}
	return m_iCapacity;
}

HM_TEMPLATE_DEF
hash_t THashMap::lastIndex() {
	hash_t i = m_iCapacity - 1;
	for(; i >= 0; --i) {
		if(m_vHashTable[i])
			return i;
	}
	return m_iCapacity;
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
