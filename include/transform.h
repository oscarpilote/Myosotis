#pragma once

#include "affine.h"
#include "quat.h"
#include "mat4.h"

template <typename T> struct TRigid;
typedef TRigid<float> Rigid;

template <typename T>
struct TRigid {

	TQuat<T> rot;
	TVec3<T> trans;

	TRigid inv() const;
	TMat4<T> as_matrix() const;

	static constexpr TRigid Identity = {TQuat<T>::Identity, TVec3<T>::Zero};
};

template <typename T>
TVec3<T> rotate(const TVec3<T>& v, const TQuat<T>& r);  

template <typename T>
TVec3<T> unrotate(const TVec3<T>& v, const TQuat<T>& r);  

template <typename T>
TVec3<T> orbit(const TVec3<T>& point, const TQuat<T>& r, const TVec3<T>& pivot);

template <typename T>
TQuat<T> compose(const TQuat<T>& r1, const TQuat<T>& r2);

template <typename T>
TRigid<T> compose(const TQuat<T>& r, const TVec3<T>& t);

template <typename T>
TRigid<T> compose(const TVec3<T>& t, const TQuat<T>& r);

template <typename T>
TRigid<T> compose(const TRigid<T>& rt1, const TRigid<T>& rt2);

template <typename T>
TMat4<T> compose(const TMat4<T>& m1, const TMat4<T>& m2);

template <typename T>
TQuat<T> great_circle_rotation(const TVec3<T>& from, const TVec3<T>& to);

/* Implementations */

template <typename T>
TMat4<T> TRigid<T>::as_matrix() const
{
}

template <typename T>
inline TVec3<T> rotate(const TVec3<T>& v, const TQuat<T>& q)
{
	assert(approx_equal<T>(norm(q), 1));
	
	return 2.f * dot(q.xyz, v) * q.xyz + (2.f * q.w * q.w - 1.f) * v +
		2.f * q.w * cross(q.xyz, v);
}

template <typename T>
inline TVec3<T> unrotate(const TVec3<T>& v, const TQuat<T>& q)
{
	assert(approx_equal<T>(norm(q), 1));
	
	return (2.f * dot(q.xyz, v) * q.xyz) + ((2.f * q.w * q.w - 1.f) * v) -
		(2.f * q.w * cross(q.xyz, v));
}

template <typename T>
TVec3<T> orbit(const TVec3<T>& point, const TQuat<T>& q, const TVec3<T>& pivot)
{
	return rotate(point - pivot, q) + pivot;
}

template <typename T>
TQuat<T> compose(const TQuat<T>& r1, const TQuat<T>& r2)
{
	return r2 * r1;
}

template <typename T>
TRigid<T> compose(const TQuat<T>& r, const TVec3<T>& t)
{
	return {r, t};
}

template <typename T>
TRigid<T> compose(const TVec3<T>& t, const TQuat<T>& r)
{
	return {r, rotate(t, r)};
}

template <typename T>
TRigid<T> compose(const TRigid<T>& rt1, const TRigid<T>& rt2)
{
	return {rt2.rot * rt1.rot, rotate(rt1.trans, rt2.rot) + rt2.trans};
}

template <typename T>
TMat4<T> compose(const TMat4<T>& m1, const TMat4<T>& m2)
{
	return m2 * m1;
}

template <typename T>
inline TQuat<T> great_circle_rotation(const TVec3<T>& from, const TVec3<T>& to)
{
	assert(approx_equal<T>(norm(from), 1));
	assert(approx_equal<T>(norm(to), 1));

	T cos_angle = dot(from, to);
	if (approx_equal<T>(cos_angle, -1)) 
	{
		return {{0, 0, 1}, 0};
	}

	T cos_half_angle = sqrt((1.f + cos_angle) / 2.f);

	assert(cos_half_angle != 0);
	
	T w = cos_half_angle;
	TVec3<T> xyz = cross(from, to) * (0.5f / cos_half_angle);
	
	return {xyz, w};
}


