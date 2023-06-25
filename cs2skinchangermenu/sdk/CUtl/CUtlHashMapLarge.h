#pragma once

#include "CUtlString.h"

template <typename T>
class CDefEquals
{
public:
	CDefEquals() {}
	CDefEquals(int i) {}
	inline bool operator()(const T& lhs, const T& rhs) const { return (lhs == rhs); }
	inline bool operator!() const { return false; }
};


// Specialization to compare pointers
template <typename T>
class CDefEquals<T*>
{
public:
	CDefEquals() {}
	CDefEquals(int i) {}
	inline bool operator()(const T* lhs, const T* rhs) const
	{
		if (lhs == rhs)
			return true;
		else if (NULL == lhs || NULL == rhs)
			return false;
		else
			return (*lhs == *rhs);
	}
	inline bool operator!() const { return false; }
};

//-----------------------------------------------------------------------------
//
// Purpose:	An associative container. Pretty much identical to CUtlMap without the ability to walk in-order
//	This container is well suited for large and growing datasets. It uses less 
//	memory than other hash maps and incrementally rehashes to reduce reallocation spikes.
//	However, it is slower (by about 20%) than CUtlHashTable
//
//-----------------------------------------------------------------------------
template <typename K, typename T, typename L = CDefEquals<K>, typename H = CUtlString > // no fucking clue how to recreate murmur3
class CUtlHashMapLarge : public base_utlmap_t
{
public:
	// This enum exists so that FOR_EACH_MAP and FOR_EACH_MAP_FAST cannot accidentally
	// be used on a type that is not a CUtlMap. If the code compiles then all is well.
	// The check for IsUtlMap being true should be free.
	// Using an enum rather than a static const bool ensures that this trick works even
	// with optimizations disabled on gcc.
	enum CompileTimeCheck
	{
		IsUtlMap = 1
	};

	typedef K KeyType_t;
	typedef T ElemType_t;
	typedef int IndexType_t;
	typedef L EqualityFunc_t;
	typedef H HashFunc_t;

	CUtlHashMapLarge()
	{
		m_cElements = 0;
		m_nMaxElement = 0;
		m_nMinRehashedBucket = InvalidIndex();
		m_nMaxRehashedBucket = InvalidIndex();
		m_iNodeFreeListHead = InvalidIndex();
	}

	CUtlHashMapLarge(int cElementsExpected)
	{
		m_cElements = 0;
		m_nMaxElement = 0;
		m_nMinRehashedBucket = InvalidIndex();
		m_nMaxRehashedBucket = InvalidIndex();
		m_iNodeFreeListHead = InvalidIndex();
		EnsureCapacity(cElementsExpected);
	}

	~CUtlHashMapLarge()
	{
		RemoveAll();
	}

	// gets particular elements
	ElemType_t& Element(IndexType_t i) { return m_memNodes.Element(i).m_elem; }
	const ElemType_t& Element(IndexType_t i) const { return m_memNodes.Element(i).m_elem; }
	ElemType_t& operator[](IndexType_t i) { return m_memNodes.Element(i).m_elem; }
	const ElemType_t& operator[](IndexType_t i) const { return m_memNodes.Element(i).m_elem; }
	KeyType_t& Key(IndexType_t i) { return m_memNodes.Element(i).m_key; }
	const KeyType_t& Key(IndexType_t i) const { return m_memNodes.Element(i).m_key; }

	// Num elements
	IndexType_t Count() const { return m_cElements; }

	// Max "size" of the vector
	IndexType_t  MaxElement() const { return m_nMaxElement; }

	// Checks if a node is valid and in the map
	bool  IsValidIndex(IndexType_t i) const { return i >= 0 && i < m_nMaxElement && !IsFreeNodeID(m_memNodes[i].m_iNextNode); }

	// Invalid index
	static IndexType_t InvalidIndex() { return -1; }

	// Insert method
	IndexType_t  Insert(const KeyType_t& key, const ElemType_t& insert) { return InsertInternal(key, insert, eInsert_UpdateExisting); }
	IndexType_t  Insert(const KeyType_t& key) { return InsertInternal(key, ElemType_t(), eInsert_UpdateExisting); }
	IndexType_t  InsertWithDupes(const KeyType_t& key, const ElemType_t& insert) { return InsertInternal(key, insert, eInsert_CreateDupes); }
	IndexType_t	 FindOrInsert(const KeyType_t& key, const ElemType_t& insert) { return InsertInternal(key, insert, eInsert_LeaveExisting); }
	IndexType_t  InsertOrReplace(const KeyType_t& key, const ElemType_t& insert) { return InsertInternal(key, insert, eInsert_UpdateExisting); }


	// Finds an element
	IndexType_t  Find(const KeyType_t& key) const;

	// has an element
	bool HasElement(const KeyType_t& key) const
	{
		return Find(key) != InvalidIndex();
	}

	void EnsureCapacity(int num);

	void RemoveAt(IndexType_t i);
	bool Remove(const KeyType_t& key)
	{
		int iMap = Find(key);
		if (iMap != InvalidIndex())
		{
			RemoveAt(iMap);
			return true;
		}
		return false;
	}
	void RemoveAll();
	void Purge();
	void PurgeAndDeleteElements();

	void Swap(CUtlHashMapLarge<K, T, L, H>& rhs)
	{
		m_vecHashBuckets.Swap(rhs.m_vecHashBuckets);
		V_swap(m_bitsMigratedBuckets, rhs.m_bitsMigratedBuckets);
		m_memNodes.Swap(rhs.m_memNodes);
		V_swap(m_iNodeFreeListHead, rhs.m_iNodeFreeListHead);
		V_swap(m_cElements, rhs.m_cElements);
		V_swap(m_nMaxElement, rhs.m_nMaxElement);
		V_swap(m_nMinRehashedBucket, rhs.m_nMinRehashedBucket);
		V_swap(m_nMaxRehashedBucket, rhs.m_nMaxRehashedBucket);
		V_swap(m_EqualityFunc, rhs.m_EqualityFunc);
		V_swap(m_HashFunc, rhs.m_HashFunc);
	}

private:
	enum EInsertPolicy { eInsert_UpdateExisting, eInsert_LeaveExisting, eInsert_CreateDupes };
	IndexType_t InsertInternal(const KeyType_t& key, const ElemType_t& insert, EInsertPolicy ePolicy);

	inline IndexType_t FreeNodeIDToIndex(IndexType_t i) const { return (0 - i) - 3; }
	inline IndexType_t FreeNodeIndexToID(IndexType_t i) const { return (-3) - i; }
	inline bool IsFreeNodeID(IndexType_t i) const { return i < InvalidIndex(); }

	int FindInBucket(int iBucket, const KeyType_t& key) const;
	int AllocNode();
	void RehashNodesInBucket(int iBucket);
	void LinkNodeIntoBucket(int iBucket, int iNewNode);
	void UnlinkNodeFromBucket(int iBucket, int iNewNode);
	bool RemoveNodeFromBucket(int iBucket, int iNodeToRemove);
	void IncrementalRehash();

	struct HashBucket_t
	{
		IndexType_t m_iNode;
	};
	CUtlVector<HashBucket_t> m_vecHashBuckets;

	CLargeVarBitVec m_bitsMigratedBuckets;

	struct Node_t
	{
		KeyType_t m_key;
		ElemType_t m_elem;
		int m_iNextNode;
	};
	CUtlMemory<Node_t> m_memNodes;
	IndexType_t m_iNodeFreeListHead;

	IndexType_t m_cElements;
	IndexType_t m_nMaxElement;
	IndexType_t m_nMinRehashedBucket, m_nMaxRehashedBucket;
	EqualityFunc_t m_EqualityFunc;
	HashFunc_t m_HashFunc;
};