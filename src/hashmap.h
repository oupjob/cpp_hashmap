#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <list>
#include <initializer_list>
#include <pair>

#include "hash_funcions.h"

template <
	hm_size_t modulo,  
	typename KeyT,
	typename T,
	typename hashFunction = stdHashFunction
> 
class HashMap 
{
public:
	typedef T*					PtrType;
	typedef T&					RefType;
	typedef KeyT*				PtrKeyType;
	typedef KeyT&				RefKeyType;
	typedef std::list<T> 		ListType;
	typedef 
	std::list<const RefType> 	RefTypeList;
	typedef list_type*			HashTableType;
	typedef std::pair<keyT, T> 	PairType;
	
private:
	HashTableType		m_pHashTable;
	hm_size_t			m_iModulo;
	hm_size_t			m_iSize;
	
public:
	HashMap();
	HashMap(std::initializer_list<T>& l_Init);
	HashMap(const HashMap& src);
	
	void extend(const hm_size_t i_NewModulo);
	
	PairType find(cosnt ref_key_type tKeyRef) const;
	T& operator [] (cosnt ref_key_type tKeyRef) const;
	
	list_cRefType equalRange(const ref_key_type t_Key) const;
	
	void insert(const ref_key_type tKey, const RefType tValue);
	void insert(const PairType& pair); 
	
	PairType remove(const ref_key_type tKey);
	
	bool 		empty() const;
	hm_size_t 	size() const;
	hm_size_t 	modulo() const;
	
	void clear();
};

#endif
