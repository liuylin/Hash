#pragma once

#include<iostream>
using namespace std;
#include<vector>
#include<string>
#include<time.h>

template<class ValueType>
struct HashNode
{
	/*K _key;
	V _value;*/

	/*HashNode(const K& key, const V& value)
		:_key(key)
		, _value(value)
		, _next(NULL)
	{}*/

	ValueType _valueField;
    HashNode<ValueType>* _next;
	HashNode(const ValueType& v)
		:_valueField(v)
		,_next(NULL)
	{}
};

//默认是整型
template<class K>
struct __HashFunc
{
	size_t operator()(const K& key)
	{
		return (size_t)key;
	}
};

struct _HashFuncString
{
	static size_t BKDRHash(const char* str)
	{
		unsigned int seed = 131;// 31 131 1313 13131 131313
		unsigned int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return hash;
	}

	size_t operator()(const string& s)
	{
		return BKDRHash(s.c_str());
	}
};

//迭代器
//前置声明
template<class K, class ValueType, class KeyOfValue, class _HashFunc = __HashFunc<K>>
class HashTable;

template <class K, class ValueType, class KeyOfValue, class _HashFunc = __HashFunc<K>>
struct __HashTableIterator
{
	typedef HashNode<ValueType> Node;
	typedef __HashTableIterator<K, ValueType, KeyOfValue, _HashFunc> Self;
	Node* _node;
	HashTable<K, ValueType, KeyOfValue, _HashFunc>* _hashtable;
	__HashTableIterator(Node* node, HashTable<K, ValueType, KeyOfValue, _HashFunc>* hashtable)
		:_node(node)
		,_hashtable(hashtable)
	{}

	ValueType& operator*()
	{
		return _node->_valueField;
	}

	ValueType* operator->()
	{
		return &(operator*());
	}

	bool operator == (const Self& s)const 
	{
		return _node == s._node;
	}

	bool operator != (const Self& s)const 
	{
		return _node != s._node;
	}

	Self& operator++()
	{
		if (_node->_next != NULL)
		{
			_node = _node->_next;
		}
		else
		{
			KeyOfValue keyofvalue;
			size_t index = _hashtable->HashFunc(keyofvalue(_node->_valueField),
				_hashtable->_tables.size());
			Node* cur = NULL;
			//为什么是index+1?因为考虑到可能是hashtable最后一个节点,则不需要再++
			for (size_t i = index+1; i < _hashtable->_tables.size(); ++i)
			{//如果不为NULL
				if (_hashtable->_tables[i])
				{
					cur = _hashtable->_tables[i];
					break;
				}
			}
			_node = cur;
		}
		return *this;
	}

	Self operator++(int)
	{
		Self tmp(*this);
		++tmp;
		return *this;
	}
};

//set
template<class K>
struct __KeyOfK
{
	const K& operator()(const K& key)
	{
		return key;
	}
};
//map
template<class K,class V>
struct __KeyOfKV
{
	const K& operator()(const pair<K, V>& kv)
	{
		return kv.first;
	}
};
//UnOrderedMap<K,V> -> HashTable<K,pair<K,V>,__KeyOfKV<K,V>>
//UnOrderedSet<K>   -> HashTable<K,K,__KeyOfK<K>>

template<class K, class ValueType, class KeyOfValue, class _HashFunc = __HashFunc<K>>
class HashTable
{
	typedef  HashNode<ValueType> Node;
	friend class __HashTableIterator<K, ValueType, KeyOfValue, _HashFunc>;
public:
	typedef __HashTableIterator<K, ValueType, KeyOfValue, _HashFunc> Iterator;
	HashTable()
		:_size(0)
	{}

	~HashTable()
	{
		for (size_t i = 0; i < _tables.size(); ++i)
		{
			Node* cur = _tables[i];
			while (cur)
			{
				Node* next = cur->_next;
				delete cur;
				cur = next;
			}
			_tables[i] = NULL;
		}
	}

	//bool InSert(const K& key, const V& value)
	//{
	//	CheckCapacity();
	//	size_t index = HashFunc(key,_tables.size());
	//	Node* cur = _tables[index];
	//	while (cur)
	//	{
	//		if (cur->_key == key)
	//		{
	//			return false;
	//		}
	//		cur = cur->_next;
	//	}
	//	//头插
	//	Node* tmp = new Node(key, value);
	//	tmp->_next = _tables[index];
	//	_tables[index] = tmp;
	//	++_size;
	//}

	pair<Iterator,bool> InSert(const ValueType& kv)
	{
		CheckCapacity();
		KeyOfValue keyofvalue;
		size_t index = HashFunc(keyofvalue(kv), _tables.size());
		Node* cur = _tables[index];
		while (cur)
		{
			if (keyofvalue(cur->_valueField) == keyofvalue(kv))
			{
				return make_pair(Iterator(cur,this),false);
			}
			cur = cur->_next;
		}
		//头插
		Node* tmp = new Node(kv);
		tmp->_next = _tables[index];
		_tables[index] = tmp;
		++_size;
		return make_pair(Iterator(tmp,this),true);
	}

	//素数表
	size_t GetNextPrime(size_t value)
	{
		const int _PrimeSize = 28;
		static const unsigned long _PrimeList[_PrimeSize] =
		{
			53ul, 97ul, 193ul, 389ul, 769ul,
			1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
			49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
			1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
			50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};
		for (size_t i = 0; i < _PrimeSize; ++i)
		{
			if (_PrimeList[i] > value)
				return _PrimeList[i];
		}
		return _PrimeList[_PrimeSize - 1];
	}


	void CheckCapacity()
	{
		if (_size == _tables.size())
		{
			KeyOfValue keyofvalue;
			vector<Node*> newTables;
			newTables.resize(GetNextPrime(_tables.size()));
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				Node* cur = _tables[i];
				while (cur)
				{
					Node* next = cur->_next;
					//头插
					size_t index = HashFunc(keyofvalue(cur->_valueField), newTables.size());
					cur->_next = newTables[index];
					newTables[index] = cur;

					cur = next;
				}
				_tables[i] = NULL;
			}
			_tables.swap(newTables);
		}
	}

	Iterator Find(const K& key)
	{
		KeyOfValue keyofvalue;
		size_t index = HashFunc(key, _tables.size());
		Node* cur = _tables[index];
		while (cur)
		{
			if (keyofvalue(cur->_valueField)== key)
				return Iterator(cur,this);
			cur = cur->_next;
		}
		return End();
	}

	void Erase(Iterator pos)
	{
		KeyOfValue keyofvalue;
		size_t index = HashFunc(keyofvalue(*pos), _tables.size());
		Node* prev = NULL;
		Node* cur = _tables[index];
		while (cur)
		{
			if (keyofvalue(cur->_valueField) == key)
			{
				if (prev == NULL)//cur是头要特殊处理
				{
					_tables[index] = cur->_next;
				}
				else
				{
					prev->_next = cur->_next;
				}
				delete cur;
				--_size;
			}
			prev = cur;
			cur = cur->_next;
		}
	}

	size_t Erase(const K& key)
	{
		Iterator pos = Find(key);
		if (pos != End())
		{
			Erase(pos);
			return 1;
		}
		else
			return 0;
	}

	size_t HashFunc(const K& key,size_t size)
	{
		_HashFunc hushfuc;
		return hushfuc(key)%size;
	}

	Iterator Begin()
	{//返回第一个不为NULL的节点
		Node* cur = NULL;
		for (size_t i = 0; i < _tables.size(); ++i)
		{
			if (_tables[i])
			{
				cur = _tables[i];
				break;
			}
		}
		return Iterator(cur, this);//不是单参数的构造函数不能进行隐式类型转换
	}

	Iterator End()
	{
		return Iterator(NULL, this);
	}
private:
	vector<Node*> _tables;//vector里面存的是节点
	size_t _size;
};

void TestHashTable()
{
	HashTable<int, int,__KeyOfK<int>> ht1;//set
	ht1.InSert(1);
	ht1.InSert(5);
	ht1.InSert(1);
	ht1.InSert(8);
	HashTable<int, int, __KeyOfK<int>>::Iterator it1 = ht1.Begin();
	while (it1 != ht1.End())
	{
		cout << *it1 << " ";
		++it1;
	}
	cout << endl;
	//返回当前时间的时间戳
	/*srand(time(0));
	for (size_t i = 0; i < 53; ++i)
	{
		ht1.InSert(rand(), 0);
	}*/

	HashTable<string, pair<string, string>, __KeyOfKV<string, string>, _HashFuncString> dict;//map
	dict.InSert(make_pair("left", "左边"));
	dict.InSert(make_pair("right", "右边"));
}

