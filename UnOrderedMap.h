#include"HashLink.h"

template<class K, class V, class _HashFunc = __HashFunc<K>>
class UnOrderedMap
{
	typedef HashNode<pair<K,V>> Node;
public:
	typedef typename HashTable<K, pair<K, V>, __KeyOfKV<K, V>, __HashFunc<K>>::Iterator Iterator;
	pair<Iterator,bool> InSert(const pair<K,V>& kv)
	{
		return ht.InSert(kv);
	}

	Iterator Find(const K& key)
	{
		return ht.Find(key);
	}

	void Erase(Iterator pos)
	{
		return ht.Erase(pos);
	}
private:
	HashTable<K, pair<K,V>, __KeyOfKV<K,V>, __HashFunc<K>> ht;
};