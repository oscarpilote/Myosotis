#include "Vec3.h"
#include "Quat.h"
#include "Trackball.h"

#include <stdio.h>

FTrackball::FTrackball(float Width, float Height, float Radius) : 
	Width{Width}, Height{Height}, Radius{Radius}, 
	SavedPosition{0.f, 0.f, 1.f} {}


/* Using (inverse) stereographic projection from a sphere of given
 * radius and tangent to the screen plane at position (width/2, height/2).
 * Click coordinates are assumed to go increasing from left to right
 * and from top to bottom, so (0,0) is upper left. */
FVec3 FTrackball::VectAtClick(float X, float Y) const
{
	float XX = (X - 0.5f * Width) / Radius;
	float YY = (0.5 * Height - Y) / Radius;
	float A = 2.f / (1.f + XX * XX + YY * YY);

	FVec3 V {A * XX, A * YY, -1.f + A};
	
	assert(AreApproxEqual<float>(V.Norm(), 1.f));

	return V;
}

void FTrackball::TrackFrom(float X, float Y)
{
	SavedPosition = VectAtClick(X, Y);
}

FQuat FTrackball::GetRotationTo(float X, float Y) const
{
	FVec3 NewPosition = VectAtClick(X, Y);

	return GreatCircleRotation(SavedPosition, NewPosition);
}

void FTrackball::Resize(float Width, float Height, float Radius)
{
	this->Width = Width;
	this->Height = Height;
	this->Radius = Radius;
}
