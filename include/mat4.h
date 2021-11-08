#pragma once

#include <assert.h>

template <typename T>
struct TMat4 {

	T Coeff[4][4];

	/* Beware of reverse ordering of indices for coeffcients access : 
	 *
	 *                M(i,j) = M.coeff[j][i]  
	 * 
	 * In practice in code we can then think as we do in math: M(i,j) 
	 * refers to coefficient in row i and colum j of the matrix M. 
	 * Since C++ uses row major ordering for multi-dimensional arrays, 
	 * this trick has the effect of turning it into a column major 
	 * ordering (which a number of well established APIs expect).
	 */
	T& operator()(int i, int j);
	const T& operator()(int i, int j) const;

};

typedef TMat4<float> Mat4;

template <typename T>
inline T& TMat4<T>::operator()(int i, int j)
{
	assert(i >= 0 && i <= 3 && j >= 0 && j <= 3);

	return Coeff[j][i];
}

template <typename T>
inline const T& TMat4<T>::operator()(int i, int j) const
{
	assert(i >= 0 && i <= 3 && j >= 0 && j <= 3);

	return Coeff[j][i];
}

