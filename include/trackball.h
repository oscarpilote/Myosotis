#ifndef _TRACKBALL_H
#define _TRACKBALL_H

#include <assert.h>

#include "Vec3.h"
#include "Quat.h"

struct FTrackball {
	float Width;
	float Height;
	float Radius;
	FVec3 SavedPosition;
	
	FTrackball() = default;
	FTrackball(float Width, float Height, float Radius);
	FVec3 VectAtClick(float X, float Y) const;
	void TrackFrom(float X, float Y);
	FQuat GetRotationTo(float X, float Y) const;
	void Resize(float Width, float Height, float Radius);
};


#endif /* _TRACKBALL_H */
