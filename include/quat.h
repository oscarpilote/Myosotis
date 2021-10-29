#include <assert.h>

#include "math_utils.h"
#include "vec3.h"

template <typename T>
struct alignas(4 * sizeof(T)) Quat {
	T r;
	Vec3<T> V;

	/* Constructors */
	Quat(T r, Vec3<T> V);

	/* Norm */
	T dot(const Quat& Q) const;
	T norm() const;

	/* Multiplication */
	Quat operator* (const Quat& Q) const;
	Quat operator* (T a) const;
	
	/* Conjugate and inverse   */
	/* TODO Quat conj() const; */
	/* TODO Quat inv() const;  */
};

template <typename T>
inline
Quat<T>::Quat(T r, Vec3<T> V) : r{r}, V{V} {};

template <typename T>
inline
T Quat<T>::dot(const Quat& Q) const
{
	return r * Q.r + V.dot(Q.V);
}

template <typename T>
inline
T Quat<T>::norm() const
{
	return std::sqrt(r * r + V.dot(V));
}

template <typename T>
inline
Quat<T> Quat<T>::operator*(const Quat& Q) const
{
	return {r * Q.r - V.dot(Q.V),
		r * Q.V + V * Q.r + V.cross(Q.V)};
}

template <typename T>
inline
Quat<T> Quat<T>::operator*(T a) const
{
	return {r * a, V * a};
}

template <typename T>
Quat<T> great_circle_rotation(const Vec3<T>& from, const Vec3<T>& to)
{
	assert(approx_equal<T>(from.norm(), 1));
	assert(approx_equal<T>(  to.norm(), 1));

	/* a = angle between from and to */
	T cos_a = from.dot(to);
	if (approx_equal<T>(cos_a, -1)) 
	{
		return {0, {0, 0, 1}};
	}

	T cos_half_a = std::sqrt((1.f + cos_a) / 2.f);

	assert(cos_half_a != 0);
	
	T sin_half_a_over_sin_a = 0.5f / cos_half_a;

	T r = cos_half_a;
	Vec3<T> V = from.cross(to) * sin_half_a_over_sin_a;
	
	return {r, V};
}

