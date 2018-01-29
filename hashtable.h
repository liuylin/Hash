#pragma once

#include<iostream>
using namespace std;
#include<vector>
#include<string>
#include<time.h>

//线性探测
enum State
{
	EMPTY,//NULL
	EXIST,//存在
	DELETE,//删除
};

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

template<class K,class V>
struct HashNode
{
	State _state;
	K _key;
	V _value;
};


template<class K,class V,class _HashFunc = __HashFunc<K>>
class HashTable
{
	typedef HashNode<K, V> Node;
public:
	HashTable()
		:_size(0)
	{}


	bool InSert(const K& key, const V& value)
	{
		CheckCapacity();
		size_t index = HashFunc(key);
		size_t first = index;
		size_t i = 1;

		while (_tables[index]._state == EXIST)
		{
			if (_tables[index]._key == key)
			{
				if (_tables[index]._state == EXIST)
				{
					return  false;
				}
			}
			//index += (first + i * i)%_tables.size();
			//++i;
			++index;
			if (index == _tables.size())
			{
				index = 0;
			}
		}
		_tables[index]._key = key;
		_tables[index]._value = value;
		_tables[index]._state = EXIST;
		++_size;
		return true;
	}

	Node* Find(const K& key)
	{
		size_t index = HashFunc(key);
		while (_tables[index]._state != EMPTY)
		{
			if (_tables[index]._key == key)
			{
				return &_tables[index];
			}
			++index;
			if (index == _tables.size())
			{
				index = 0;
			}
		}
		return NULL;
	}

	bool Erase(const K& key)
	{
		HashNode* ret = Find(key);
		if (ret)
		{
			//伪删除法
			--_size;
			ret->_state = DELETE;
		}
		else
			return false;
	}

	size_t GetNextPrime(size_t value)
	{
		const int _PrimeSize = 28;
		static const unsigned long _PrimeList[_PrimeSize] =
		{
			53ul, 97ul, 193ul, 389ul, 769ul,
			1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
			49157ul, 98317ul, 196613ul, 393241ul,786433ul,
			1572869ul, 3145739ul, 6291469ul, 12582917ul,25165843ul,
			50331653ul, 100663319ul, 201326611ul, 402653189ul,805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};
		for (size_t i = 0; i < _PrimeList; ++i)
		{
			if (_PrimeList[i] > value)
				return _PrimeList[i];
		}
		return _PrimeList[_PrimeSize - 1];
	}

	void CheckCapacity()
	{
		if (_tables.size() == 0 || _size*10 / _tables.size() >= 7)
		{
			/*vector<Node> newTables;
			newTables.resize(newSize);*/
			HashTable<K, V,_HashFunc> newTable;
			newTable._tables.resize(GetNextPrime(_tables.size());
			for (size_t i = 0; i < _tables.size(); ++i)
			{
					newTable.InSert(_tables[i]._key, _tables[i]._value);
			}
			//vector中的swap只交换指针
			_tables.swap(newTable._tables);
		}
	}

	size_t HashFunc(const K& key)
	{
		_HashFunc hashfunc;
		return hashfunc(key) % _tables.size();
	}
protected:
	vector<Node> _tables;//size是表的大小
	size_t _size;//表有效数据的个数
};



void TestHashTable()
{
	HashTable<int, int> ht1;
	ht1.InSert(89,0);
	ht1.InSert(18, 0);
	ht1.InSert(49, 0);
	ht1.InSert(58, 0);
	ht1.InSert(9, 0);

	//返回当前时间的时间戳
	srand(time(0));
	for (size_t i = 0; i < 53; ++i)
	{
		ht1.InSert(rand(), 0);
	}

	HashTable<string, string,_HashFuncString> dict;
	dict.InSert("left", "左边");
	dict.InSert("right", "右边");
}


