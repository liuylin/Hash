#include"HashLink.h"

template<class K,class _HashFunc = __HashFunc<K>>
class UnOrderedSet
{
	typedef HashNode<K> Node;
public:
	typedef typename HashTable<K, K, __KeyOfK<K>, __HashFunc<K>>::Iterator Iterator;
	pair<Iterator,bool> InSert(const K& key)
	{
		return ht.InSert(key);
	}

	size_t Erase(const K& key)
	{
		return ht.Erase(key);
	}

	Iterator Find(const K& key)
	{
		return ht.Find(key);
	}

	void Erase(Iterator pos)
	{
		ht.Erase(pos);
	}

private:
	HashTable<K, K, __KeyOfK<K>,__HashFunc<K>> ht;
};