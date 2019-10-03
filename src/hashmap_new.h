#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <utility>
#include <cstring>
#include <mutex>
#include <cmath>
#include <shared_ptr>

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
	typedef HashMapKeyTypeTraits<KeyT>				KeyTypeTraits;			
	typedef typename KeyTypeTraits::HashFuncArgType HashFuncArgType;
	
	typedef std::pair<KeyT, T> 						PairType;
	
	typedef std::shared_ptr<PairType>				SharedPtrPairType;
	typedef const SharedPtrPairType					ConstSharedPtrPairType;
	
	typedef const PairType*							ConstPtrPairType;
	
	typedef std::list<ConstSharedPtrPairType> 		EqualRangeType;
	
	typedef std::list<SharedPtrPairType>			DanglingChainRangeType;
	
	typedef std::shared_lock<std::shared_mutex>		ReadLockType;
	typedef std::lock_guard<std::shared_mutex>		WriteLockType;
	
	typedef std::shared_ptr<PairType>*				HashTableType;

	
private:
	HashTableType			m_vHashTable;
	size_t					m_iCapacity;
	std::shared_mutex		m_mSharedMutex;
	
	hash_t firstIndex();
	hash_t lastIndex();
	
	hash_t findHashOfBusyCell(HashFuncArgType tKey, size_t iPos = 0) const;
	
	size_t equalRangeImpl(HashFuncArgType tKey, EqualRangeType& lOut) const;
	size_t equalRangeImplForPos(HashFuncArgType tKey, size_t iPos, size_t nCount, EqualRangeType& lOut) const;	
	
	void buildDanglingChain(DanglingChainRangeType& out);
	
	void extendImpl(const size_t iNewCapacity);
	
public:
	HashMap();
	HashMap(const std::initializer_list<PairType>& lInit);
	HashMap(const HashMap<KeyT, T, hashFunction>& oSrc);
	~HashMap();
	
	ConstSharedPtrPairType find(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0) const;
	
	EqualRangeType operator [] (HashFuncArgType tKey) const;
	EqualRangeType equalRange(HashFuncArgType tKey, size_t iPos=0, size_t nCount=0) const;
	
	void insert(HashFuncArgType tKey, ConstRefType tValue);
	void insert(ConstRefPairType pair); 
	
	PairType remove(HashFuncArgType tKey, ConstRefPairType oDefaultPair, size_t iPos = 0);
	
	void extend(const size_t iNewCapacity);
	
	bool 		empty() const { return !size(); } 
	size_t		capacity() const { return m_iCapacity; }
	bool		isDefaultPair(RefPairType oPair, RefPairType oDefaultPair);
	size_t 		size() const;
	
	void clear();
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

#define is_valid_hash(h) h < m_iCapacity
#define is_invalid_hash(h) h >= m_iCapacity

HM_TEMPLATE_DECL THashMap::HashMap() 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	
	m_vHashTable = new PtrPairType[m_iCapacity];
	std::memset(m_vHashTable, 0, sizeof(PtrPairType) * m_iCapacity);
}

HM_TEMPLATE_DECL THashMap::HashMap(const std::initializer_list<THashMap::PairType>& lInit) 
{
	m_iCapacity = DEFAULT_CAPACITY;	
	
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

HM_TEMPLATE_DECL THashMap::HashMap(const THashMap& oSrc) 
{	
	m_iCapacity = oSrc.m_iCapacity;	
	m_vHashTable = new PairType*[m_iCapacity];
	
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (oSrc.m_vHashTable[i])
			m_vHashTable[i] = new PairType((*oSrc.m_vHashTable[i]));
		else
			m_vHashTable[i] = nullptr;
	}
}

HM_TEMPLATE_DECL THashMap::~HashMap()
{
	clear();
	delete [] m_vHashTable;
}

HM_TEMPLATE_DECL
bool THashMap::isDefaultPair(RefPairType oPair, RefPairType oDefaultPair) 
{
	return key(oPair) == key(oDefaultPair) && value(oPair) == value(oDefaultPair); 
}

HM_TEMPLATE_DECL 
hash_t THashMap::findHashOfBusyCell(const typename THashMap::HashFuncArgType tKey, size_t iPos) const
{	
	PairType* pLast = nullptr;
	size_t iHits = 0;
	hash_t h;
			
	if (iPos >= m_iCapacity) return m_iCapacity;
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

HM_TEMPLATE_DECL
typename THashMap::ConstRefPairType 
THashMap::find(
	THashMap::HashFuncArgType tKey, THashMap::ConstRefPairType oDefault, size_t iPos
) const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
		
	hash_t h = findHashOfBusyCell(tKey, iPos);
	if (is_valid_hash(h) && key_ptr(m_vHashTable[h]) == tKey) {
		return *m_vHashTable[h];
	} else {
		return oDefault;
	}
}

HM_TEMPLATE_DECL
size_t THashMap::equalRangeImpl(typename THashMap::HashFuncArgType tKey, typename THashMap::EqualRangeType& lOut) const 
{	
	size_t ik = 0;
	for(; ik < m_iCapacity; ++ik) {
		hash_t h = hashFunction(m_iCapacity, tKey, ik);
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

HM_TEMPLATE_DECL
typename THashMap::EqualRangeType 
THashMap::equalRange(const typename THashMap::HashFuncArgType tKey, size_t iPos, size_t nCount) const 
{	
	ReadLockType	lReadLock(m_mSharedMutex);
	
	EqualRangeType lEqualRange;
	
	if (!iPos && !nCount) {
		equalRangeImpl(tKey, lEqualRange);
	} else {
		equalRangeImplForPos(tKey, iPos, nCount, lEqualRange);
	}
		
	return lEqualRange;
}

HM_TEMPLATE_DECL 
typename THashMap::EqualRangeType 
THashMap::operator [] (const typename THashMap::HashFuncArgType tKey) const 
{
	ReadLockType	lReadLock(m_mSharedMutex);
	EqualRangeType lEqualRange;
	
	equalRangeImpl(tKey, lEqualRange);
	return lEqualRange;
}

HM_TEMPLATE_DECL  
void THashMap::insert(const typename THashMap::HashFuncArgType tKey, typename THashMap::ConstRefType tValue)
{
	WriteLockType	lWriteLock(m_mSharedMutex);
	hash_t h;
		
	while(true) 
	{
		for(size_t ik = 0; ik < m_iCapacity; ++ik) 
		{
			h = hashFunction(m_iCapacity, tKey, ik);
			if (cell_is_free(m_vHashTable[h])) {
				m_vHashTable[h] = new PairType(tKey, tValue);
				return;
			}
		}
		
		extendImpl(m_iCapacity + DEFAULT_CAPACITY);
	}
}

HM_TEMPLATE_DECL
void THashMap::insert(typename THashMap::ConstRefPairType pair) 
{
	insert(key(pair), value(pair));
}

HM_TEMPLATE_DECL
typename THashMap::EqualRangeType 
THashMap::remove(
	const typename THashMap::HashFuncArgType 	tKey,
	size_t 										iPos,
	size_t										nCount
) {
	WriteLockType	lWriteLock(m_mSharedMutex);
	
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
}
// this realization don't use insert, because insert used KeyT and T copy constructors
// so, we just copy pointers to pairs to new hash table
HM_TEMPLATE_DECL
void THashMap::extendImpl(const size_t iNewCapacity)
{	
	if (iNewCapacity <= m_iCapacity)
		return;
		
	HashTableType vNewHashTable = new PtrPairType[iNewCapacity];
	std::memset(vNewHashTable, 0, sizeof(PtrPairType) * iNewCapacity);
	
	DanglingChainRangeType lDanglingChain;
	buildDanglingChain(lDanglingChain);
		
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
	
	delete [] m_vHashTable;
	m_vHashTable = vNewHashTable;
	m_iCapacity = iNewCapacity;	
}

HM_TEMPLATE_DECL
void THashMap::extend(const size_t iNewCapacity)
{
	WriteLockType	lWriteLock(m_mSharedMutex);
	extend(iNewCapacity);
}



#define move_last(l_out, ptr) \
l_out.push_front(ptr);\
ptr = nullptr;

#define move_first(l_out, ptr) \
l_out.push_back(ptr);\
ptr = nullptr;

HM_TEMPLATE_DECL
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

HM_TEMPLATE_DECL
size_t THashMap::size() {
	size_t n = 0;
	for(size_t m = 0; m < m_iCapacity; ++m)
		n += static_cast<bool>(cell_is_busy(m_vHashTable[m]));

	return n;
}

HM_TEMPLATE_DECL
hash_t THashMap::firstIndex() {
	hash_t i = 0;
	for(; i < m_iCapacity; ++i) {
		if(m_vHashTable[i])
			return i;
	}
	return m_iCapacity;
}

HM_TEMPLATE_DECL
hash_t THashMap::lastIndex() {
	hash_t i = m_iCapacity - 1;
	for(; i >= 0; --i) {
		if(m_vHashTable[i])
			return i;
	}
	return m_iCapacity;
}

HM_TEMPLATE_DECL void THashMap::clear()
{
	for(size_t i = 0; i < m_iCapacity; ++i)
	{
		if (m_vHashTable[i]) {
			delete m_vHashTable[i];
			m_vHashTable[i] = 0;
		}
	}
	m_vHashTable = nullptr;
	m_iCapacity = 0;
}

#undef key
#undef value
#undef ptr
#undef hash
#undef cell_is_free
#undef cell_is_busy
#undef HM_TEMPLATE_DECL
#undef THashMap
#undef DEFAULT_CAPACITY

#endif
