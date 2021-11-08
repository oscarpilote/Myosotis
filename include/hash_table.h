#pragma once

#include <assert.h>
#include <stdlib.h>

#include "sys_utils.h"

/* a trivial hasher (meant for arithmetic types) */
template <typename K>
class DefaultHasher {
	size_t   hash(K key) const {return static_cast<size_t>(key);}
	bool is_empty(K key) const {return (key == ~static_cast<K>(0));}
	bool is_equal(K key1, K key2) const {return (key1 == key2);}
	K empty() const {return (~static_cast<K>(0));}
};

/* Uses OA and does _not_ implement deletion *
 * Hasher must implement hash, is_empty,     *
 * is_equal and empty.                       */
template <typename K, typename V, class H = DefaultHasher<K>>
struct HashTable {
	HashTable(size_t expected_nkeys = 0);
	~HashTable();
	V* get(K key) const;
	void set_at(K key, V val);
	float load_factor() const;
private:
	size_t size;
	size_t buckets;
	K *keys;
	V *vals;
	H hasher;
	void grow();
	bool load_factor_ok() const;
};


/* Free function not linked to the class but useful in similar contexts */ 
template<typename K, typename H>
static inline size_t hash_lookup(K *keys, size_t buckets, H hasher, K key)
{
	assert(((buckets - 1) & buckets) == 0);
	size_t mask = buckets - 1;

	size_t bucket = hasher.hash(key) & mask;

	for (size_t probe = 0; probe < buckets; probe++) {

		if (hasher.is_empty(keys[bucket]) || 
				hasher.is_equal(keys[bucket], key)) {
			return bucket;
		}
		/* quadratic probing */
		bucket = (bucket + probe + 1) & mask;
	}
	
	/* we should never reach this point */
	assert(false && "Table is full !\n"); 	
	return 0;
}

static inline size_t hashtable_buckets(size_t n)
{
	size_t ret = 1;
	while (ret < n) ret *= 2;
	return ret;
}

template<typename K, typename V, typename H>
HashTable<K, V, H>::HashTable(size_t expected_keys)
{
	size = 0;
	buckets = hashtable_buckets(expected_keys);

	keys = static_cast<K*>(malloc(buckets * sizeof(K)));
	vals = static_cast<V*>(malloc(buckets * sizeof(V)));
	assert(keys != nullptr && vals != nullptr);

	/* Initialize slots to empty keys */
	for (size_t i = 0; i < buckets; ++i)
	{
		keys[i] = hasher.empty();
		assert(hasher.is_empty(keys[i]));
	}
}

template<typename K, typename V, typename H>
HashTable<K, V, H>::~HashTable()
{
	buckets = 0;
	size = 0;

	free(keys);
	free(vals);
}

template<typename K, typename V, typename H>
bool 
HashTable<K, V, H>::load_factor_ok() const
{
	/* 66% load factor limit */
	return (buckets > size + size / 2);
};

template<typename K, typename V, typename H>
inline V* 
HashTable<K, V, H>::get(K key) const
{
	size_t bucket = hash_lookup(keys, buckets, hasher, key);
	return hasher.is_empty(keys[bucket]) ? nullptr : &vals[bucket];
}

template<typename K, typename V, typename H>
inline void 
HashTable<K, V, H>::set_at(K key, V val)
{
	size_t bucket = hash_lookup(keys, buckets, hasher, key);
	vals[bucket] = val;
	if (hasher.is_empty(keys[bucket])) {
		keys[bucket] = key;
		size++;
		if UNLIKELY(!load_factor_ok()) {
			grow();
			assert(load_factor_ok());
		}
	}
}

template<typename K, typename V, typename H>
float HashTable<K, V, H>::load_factor() const
{
	return static_cast<float>(size) / buckets;
};

template <typename K, typename V, typename H>
void 
HashTable<K, V, H>::grow()
{
	size_t new_buckets = 2 * buckets;

	assert((new_buckets & (new_buckets - 1)) == 0);
	assert(new_buckets > size);


	K *newk = (K *)malloc(new_buckets * sizeof(*newk));
	V *newv = (V *)malloc(new_buckets * sizeof(*newv));
	
	for (size_t i = 0; i < new_buckets; ++i) {
		newk[i] = hasher.empty();
	}
	
	for (size_t probe = 0; probe < buckets; ++probe) {
		
		const K key = keys[probe];
		
		if (hasher.is_empty(key)) continue;

		size_t new_idx = hash_lookup(newk, new_buckets, hasher, key);
		assert(hasher.is_empty(newk[new_idx]));

		newk[new_idx] = key;
		newv[new_idx] = vals[probe];
	}
	
	free(keys);
	free(vals);
	keys = newk;
	vals = newv;
	buckets = new_buckets;
}
