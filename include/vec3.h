#ifndef _VEC3_H
#define _VEC3_H

#include <assert.h>
#include <cmath>

template <typename T>
struct Vec3 {
	T x;
	T y;
	T z;

	/* Constructors */
	Vec3() {}; 
	Vec3(T x, T y, T z);

	/* Index Accessor */
	T operator[] (int n) const;

	/* Vector space structure */
	Vec3 operator+ (const Vec3& V) const;
	Vec3 operator- (const Vec3& V) const;
	Vec3 operator* (T a) const;

	/* Vector operations */
	T dot(const Vec3& V) const;
	T norm() const;
	Vec3 cross(const Vec3& V) const;
};

template <typename T>
inline
Vec3<T>::Vec3(T x, T y, T z) : x{x}, y{y}, z{z} {}

template <typename T>
inline
T Vec3<T>::operator[](int n) const
{
	assert(n >= 0 && n <= 2);
	return (&x)[n];
}

template <typename T>
inline
Vec3<T> Vec3<T>::operator+(const Vec3& V) const
{
	return {x + V.x, y + V.y, z + V.z};
}

template <typename T>
inline
Vec3<T> Vec3<T>::operator-(const Vec3& V) const
{
	return {x - V.x, y - V.y, z - V.z};
}

template <typename T>
inline
Vec3<T> Vec3<T>::operator*(T a) const
{
	return {a * x, a * y, a * z};
}

template <typename T>
inline
Vec3<T> operator*(T a, const Vec3<T>& V)
{
	return (V * a);
}

template <typename T>
inline
T Vec3<T>::dot(const Vec3<T>& V) const
{
	return (x * V.x + y * V.y + z * V.z);
}

template <typename T>
inline
T Vec3<T>::norm() const
{
	return std::sqrt(x * x + y * y + z * z);
}

template <typename T>
inline
Vec3<T> Vec3<T>::cross(const Vec3<T>& V) const
{
	return {y * V.z - z * V.y,
		z * V.x - x * V.z,
		x * V.y - y * V.x};
}

#endif /* _VEC3_H */
