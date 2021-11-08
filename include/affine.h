#include "vec3.h"

template <typename T> struct TPoint;
template <typename T> struct TVect;
template <typename T> struct TLine;
template <typename T> struct TPlane;


template <typename T>
struct TPoint {
	T x;
	T y;
	T z;
	T w; /* = 1 */
	
	TPoint(T x, T y, T z, T w);
	TPoint(T x, T y, T z);
	explicit TPoint(TVec3<T> v);

	TPoint operator+= (const TVect<T>& v);

	static constexpr TPoint Origin;
};


/**
 * Plucker coordinates for lines in 3D.
 * Cfr https://en.wikipedia.org/wiki/Pl%C3%BCcker_coordinates
 */
template <typename T>
struct TLine {
	union {
		struct {
			T wx;
			T wy;
			T wz;
		};
		TVec3<T> m;
	};
	union {
		struct {
			T yz;
			T zx;
			T xy;
		};
		TVec3<T> d;
	};
	
	TLine(const TPoint<T>& p, const TPoint<T>& q);
	TLine(const TPoint<T>& p, const TVect<T>& v);
};

template <typename T>
struct TPlane {
	T x;
	T y;
	T z;
	T w;
	TPlane(T x, T y, T z, T w);
};

/* Typedefs */

typedef TPoint<float> Point;
typedef TLine<float> Line;
typedef TPlane<float> Plane;

/* Implementations */

