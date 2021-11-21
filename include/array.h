#pragma once

#include <stdlib.h>
#include <assert.h>

#define ARRAY_FIRST_CAPACITY 8

template <typename T>
struct TArray {
	/**
	 * Members
	 */
	size_t size;
	size_t capacity;
	T *data;
	/**
	 * Methods
	 */
	TArray() = default;
	TArray(size_t cap);
	//~TArray();
	T& operator[] (size_t i);
	const T& operator[] (size_t i) const;
	void push_back(const T &t);
	void resize(size_t size);
	void clear();
};

//template< typename T>
//TArray<T>::TArray(): size{0}, capacity{0}, data{nullptr} {}

template <typename T>
TArray<T>::TArray(size_t cap)
{
	data = static_cast<T*>(malloc(cap * sizeof(T)));
	capacity = cap;
	size = cap;
};

//template<typename T>
//inline TArray<T>::~TArray()
//{
//	size = 0;
//	capacity = 0;
//	free(data);
//}

template<typename T>
inline T& TArray<T>::operator[](size_t i)
{
	assert(i < size);
	return (data[i]);
}

template<typename T>
inline const T& TArray<T>::operator[](size_t i) const
{
	assert(i < size);
	return (data[i]);
}

template<typename T>
inline void TArray<T>::push_back(const T &t)
{
	if (size >= capacity) {
		capacity = capacity < ARRAY_FIRST_CAPACITY ? 
			ARRAY_FIRST_CAPACITY : 2 * capacity;
		data = static_cast<T*>(realloc(data, capacity * sizeof(T)));
	}
	data[size++] = t;
}

template<typename T>
void TArray<T>::resize(size_t size)
{
	this->size = size;
	
	if (size > capacity)
	{
		data = (data != nullptr) ? 
			static_cast<T*>(realloc(data, size * sizeof(T))) :
			static_cast<T*>(malloc(size * sizeof(T)));
		capacity = size;
	}
}

template<typename T>
inline void TArray<T>::clear()
{
	size = 0;
	capacity = 0;
	free(data);
}

