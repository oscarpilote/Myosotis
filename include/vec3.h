#pragma once

#include <assert.h>
#include <cmath>

template <typename T>
struct TVec3 {
	
	/* Members */
	T x;
	T y;
	T z;
	
	/* Constructors */
	TVec3() = default; 
	TVec3(T x, T y, T z);

	/* Index Accessor */
	T& operator[] (int n);
	const T& operator[] (int n) const;

	/* Vector space structure */
	TVec3& operator-  () const;
	TVec3& operator+= (const TVec3& a);
	TVec3& operator-= (const TVec3& a);
	TVec3& operator*= (const T& t);
	TVec3& operator/= (const T& t);

	/* Static members */
	static const TVec3 Zero;
	static const TVec3 XAxis;
	static const TVec3 YAxis;
	static const TVec3 ZAxis;
};

typedef TVec3<float> Vec3;

/* Free functions declarations */

template <typename T>
inline TVec3<T> operator+ (const TVec3<T>& a, const TVec3<T>& b);  

template <typename T>
inline TVec3<T> operator- (const TVec3<T>& a, const TVec3<T>& b);  

template <typename T>
inline TVec3<T> operator* (const TVec3<T>& a, const T& t);

template <typename T>
inline TVec3<T> operator* (const T& t, const TVec3<T>& a);

template <typename T>
inline T dot(const TVec3<T>& a, const TVec3<T>& b);

template <typename T>
inline T cross(const TVec3<T>& a, const TVec3<T>& b);

template <typename T>
inline T norm(const TVec3<T> a);

/* Static Helpers */

template <typename T>
const TVec3<T> TVec3<T>::Zero  {0, 0, 0};
template <typename T>
const TVec3<T> TVec3<T>::XAxis {1, 0, 0};
template <typename T>
const TVec3<T> TVec3<T>::YAxis {0, 1, 0};
template <typename T>
const TVec3<T> TVec3<T>::ZAxis {0, 0, 1};

/* Functions implementations */

template <typename T>
inline
TVec3<T>::TVec3(T x, T y, T z) : x{x}, y{y}, z{z} {}

template <typename T>
inline
const T& TVec3<T>::operator[](int n) const
{
	assert(n >= 0 && n <= 2);
	return (&x)[n];
}

template <typename T>
inline T& TVec3<T>::operator[](int n)
{
	assert(n >= 0 && n <= 2);
	return (&x)[n];
}

template <typename T>
inline TVec3<T>& TVec3<T>::operator-() const
{
	return {-x, -y, -z};
}

template <typename T>
inline TVec3<T>& TVec3<T>::operator+= (const TVec3<T>& a)
{
	x += a.x; 
	y += a.y;
	z += a.z;
	return (*this);
}

template <typename T>
inline TVec3<T>& TVec3<T>::operator-= (const TVec3<T>& a)
{
	x -= a.x; 
	y -= a.y;
	z -= a.z;
	return (*this);
}

template <typename T>
inline TVec3<T>& TVec3<T>::operator*= (const T& t)
{
	x *= t; 
	y *= t;
	z *= t;
	return (*this);
}

template <typename T>
inline TVec3<T>& TVec3<T>::operator/= (const T& t)
{
	x /= t; 
	y /= t;
	z /= t;
	return (*this);
}

template <typename T>
inline TVec3<T> operator+ (const TVec3<T>& a, const TVec3<T>& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
inline TVec3<T> operator- (const TVec3<T>& a, const TVec3<T>& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
inline TVec3<T> operator* (const TVec3<T>& a, const T& t)
{
	return {a.x * t, a.y * t, a.z * t};
}

template <typename T>
inline TVec3<T> operator* (const T& t, const TVec3<T>& a)
{
	return a * t;
}

template <typename T>
inline T dot(const TVec3<T>& a, const TVec3<T>& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

template <typename T>
inline T cross(const TVec3<T>& a, const TVec3<T>& b)
{
	return {a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x};
}

template <typename T>
inline T norm(const TVec3<T> a)
{
	return sqrt(dot(a,a));
}
