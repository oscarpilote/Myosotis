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

	TRigid   inv() const;
	TMat4<T> as_matrix() const;

	static constexpr TRigid Identity = {TQuat<T>::Identity, TVec3<T>::Zero};
};

template <typename T>
TVec3<T> rotate(const TVec3<T>& v, const TQuat<T>& q);  

template <typename T>
TVec3<T> unrotate(const TVec3<T>& v, const TQuat<T>& q);  

template <typename T>
TVec3<T> orbit(const TVec3<T>& point, const TQuat<T>& q, const TVec3<T>& pivot);

template <typename T>
TQuat<T> compose(const TQuat<T>& q1, const TQuat<T>& q2);

template <typename T>
TRigid<T> compose(const TQuat<T>& q, const TVec3<T>& v);

template <typename T>
TRigid<T> compose(const TVec3<T>& v, const TQuat<T>& q);

template <typename T>
TRigid<T> compose(const TRigid<T>& A, const TRigid<T>& B);

template <typename T>
TMat4<T> compose(const TMat4<T>& A, const TMat4<T>& B);

template <typename T>
TVec3<T> transform(const TMat4<T>& A, const TVec3<T>& v); 

template <typename T>
TQuat<T> great_circle_rotation(const TVec3<T>& from, const TVec3<T>& to);

/* Implementations */

template <typename T>
TRigid<T> TRigid<T>::inv() const
{
	return {-rot, -unrotate(trans, rot)};

}

template <typename T>
TMat4<T> TRigid<T>::as_matrix() const
{
	TMat4<T> M;

	const T xx = rot.x * rot.x;
	const T xy = rot.x * rot.y;
	const T xz = rot.x * rot.z;
	const T xw = rot.x * rot.w;

	const T yy = rot.y * rot.y;
	const T yz = rot.y * rot.z;
	const T yw = rot.y * rot.w;

	const T zz = rot.z * rot.z;
	const T zw = rot.z * rot.w;

	M(0,0) = 1.f - 2.f * (yy + zz);
	M(1,0) = 2.f * (xy + zw);
	M(2,0) = 2.f * (xz - yw);
	M(3,0) = 0.f;

	M(0,1) = 2.f * (xy - zw);
	M(1,1) = 1.f - 2.f * (xx + zz);
	M(2,1) = 2.f * (yz + xw);
	M(3,1) = 0.f;

	M(0,2) = 2.f * (xz + yw);
	M(1,2) = 2.f * (yz - xw);
	M(2,2) = 1.f - 2.f * (xx + yy);
	M(3,2) = 0.f;

	M(0,3) = trans.x;
	M(1,3) = trans.y;
	M(2,3) = trans.z;
	M(3,3) = 1.f;

	return (M);
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
TQuat<T> compose(const TQuat<T>& q1, const TQuat<T>& q2)
{
	return q2 * q1;
}

template <typename T>
TRigid<T> compose(const TQuat<T>& q, const TVec3<T>& v)
{
	return {q, v};
}

template <typename T>
TRigid<T> compose(const TVec3<T>& v, const TQuat<T>& q)
{
	return {q, rotate(v, q)};
}

template <typename T>
TRigid<T> compose(const TRigid<T>& A, const TRigid<T>& B)
{
	return {B.rot * A.rot, rotate(A.trans, B.rot) + B.trans};
}

template <typename T>
TMat4<T> compose(const TMat4<T>& A, const TMat4<T>& B)
{
	return B * A;
}

template <typename T>
TVec3<T> transform(const TMat4<T>& A, const TVec3<T>& v)
{
	TVec4<T> prod = A(0) * v[0] + A(1) * v[1] + A(2) * v[2] + A(3);
	T inv_w = 1.f / prod.w;

	return {prod.x * inv_w, prod.y * inv_w, prod.z * inv_w};
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


