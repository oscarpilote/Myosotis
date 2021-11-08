#pragma once

#include <stdlib.h>
#include <assert.h>

#define ARRAY_FIRST_CAPACITY 8

template <typename T>
struct Array {
	size_t size;
	size_t capacity;
	T *data;
	Array();
	~Array();
	T& operator[] (size_t i);
	const T& operator[] (size_t i) const;
	void push_back(const T &t);
};

template< typename T>
inline Array<T>::Array() : size{0}, capacity{0}, data{nullptr} {}

template<typename T>
inline Array<T>::~Array()
{
	size = 0;
	capacity = 0;
	free(data);
}

template<typename T>
inline T& Array<T>::operator[](size_t i)
{
	assert(i < size);
	return (data[i]);
}

template<typename T>
inline const T& Array<T>::operator[](size_t i) const
{
	assert(i < size);
	return (data[i]);
}

template<typename T>
inline void Array<T>::push_back(const T &t)
{
	if (size >= capacity) {
		capacity = capacity < ARRAY_FIRST_CAPACITY ? 
			ARRAY_FIRST_CAPACITY : 2 * capacity;
		data = static_cast<T*>(realloc(data, capacity * sizeof(T)));
	}
	data[size++] = t;
}



