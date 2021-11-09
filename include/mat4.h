#pragma once

#include <assert.h>

#include "vec4.h"

template <typename T>
struct TMat4 {

	TVec4<T> cols[4];

	/* Element accessor and mutator */
	const T& operator()(int i, int j) const;
	T& operator()(int i, int j);
	
	/* Column acessor and mutator */
	const TVec4<T>& operator()(int i) const;
	TVec4<T>& operator()(int i);
};

typedef TMat4<float> Mat4;

/* Free functions */

template <typename T>
TMat4<T> operator* (const TMat4<T>& m1, const TMat4<T>& m2);

/* Implementations */

template <typename T>
inline const T& TMat4<T>::operator()(int i, int j) const
{
	assert(i >= 0 && i <= 3 && j >= 0 && j <= 3);

	return cols[j][i];
}

template <typename T>
inline T& TMat4<T>::operator()(int i, int j)
{
	assert(i >= 0 && i <= 3 && j >= 0 && j <= 3);

	return cols[j][i];
}

template <typename T>
inline const TVec4<T>& TMat4<T>::operator()(int j) const
{
	assert(j >= 0 && j <= 3);

	return cols[j];
}

template <typename T>
inline TVec4<T>& TMat4<T>::operator()(int j)
{
	assert(j >= 0 && j <= 3);

	return cols[j];
}

template <typename T>
TMat4<T> operator* (const TMat4<T>& A, const TMat4<T>& B)
{
	TMat4<T> AB;

	AB(0) = A(0) * B(0,0) + A(1) * B(1,0) + A(2) * B(2,0) + A(3) * B(3,0);
	AB(1) = A(0) * B(0,1) + A(1) * B(1,1) + A(2) * B(2,1) + A(3) * B(3,1);
	AB(2) = A(0) * B(0,2) + A(1) * B(1,2) + A(2) * B(2,2) + A(3) * B(3,2);
	AB(3) = A(0) * B(0,3) + A(1) * B(1,3) + A(2) * B(2,3) + A(3) * B(3,3);

	return (AB);
}
