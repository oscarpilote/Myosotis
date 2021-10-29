#include "stdio.h"



template <typename T>
struct alignas(8) Displacement {
	Quat<T> Rotation;
	Quat<T> Translation;
};




struct Camera {
	double lon;
	double lat;
	double msl;
	struct {
	   float hdg;
	   float pitch;
	   float roll;
	} base;
	float shift_x;
	float shift_y;
	float shift_z;
	struct {
	   float hdg;
	   float pitch;
	   float roll;
	} head;
	// Optical parameters of the camera. These are completly determined
	// by the left, right, top, bottom, near and far frustum planes,
	// but we use different set of parameters for user friendliness.
	float  aspect_x;	// = 2 * near/(right - left) = 1 / tan(fov_x / 2) 
	float  aspect_y;	// = 2 * near/(top - bottom) = 1 / tan(fov_y / 2)
	float  tilt_x;	    // This is 0 for normal lenses, and non zero for 
	float  tilt_y;		// so-called tilt-shift lenses; it translates the fact
						// that the optical axis is not centered over the film
						// surface. In terms of the left, right, top and bottom 
						// frustum planes, noted l, r, t, b, we have the 
						// relations tilt_x = (r+l)/(r-l) and tilt_y = (t+b)/(t-b).
						// For a non shifted lense, the horizon always
						// appears on the center of the image. For a (tilt_y) 
						// shifted_lense, the horizon will appear at Y% from the
						// bottom of the image, where Y = (1 - tilt_y) * 50. In
						// the same manner, the vertical plane through the lense
						// will appear shifted by X% from the left of the image, 
						// where X = (1 - tilt_x) * 50.
						// You may use these last relations to set tilt_x and
						// tilt_y if you want a particular X and Y.      
	float  near;		// The near frustum plane. Mostly set in real time by
						// code rather than during init.
	float  far;			// The far frustum plane. Mostly set in real time by
						// code rather than during init.
};

struct View3D {
	double Position[3];
	double Rotation[4];
	
	void SetPosition(double x, double y, double z);
	void SetRotation(double a, double b, double c, double d);
};

void View3D::SetPosition(double x, double y, double z)
{
	Position[0] = x;
	Position[1] = y;
	Position[2] = z;
}

void View3D::SetRotation(double a, double b, double c, double d)
{
	Rotation[0] = a;
	Rotation[1] = b;
	Rotation[2] = c;
	Rotation[3] = d;
}


enum ViewControllerMode {
	ORBIT,
	TURNTABLE,
	FLY,
	WALK,
	PAN,
	ZOOM
}; 

struct ViewController {
	
	View3D* View;
	double OrbitCenter[3];
	
	ViewControllerMode Mode;
	
	struct {
		double xpos;
		double ypos;
	} LastClick;

	
	float Sensitivity;

	bool bFollowMouse;

	ViewController();

	ViewControllerMode GetMode() const;

	void SetTargetView(View3D* View);
	void SetMode(ViewControllerMode Mode);
	void SetLastClick(double xpos, double ypos);
	void SetOrbitCenter(double x, double y, double z); 

	void MoveTo(double xpos, double ypos) const;
	void ScrollBy(double xoffset, double yoffset) const;
};

ViewController::ViewController() : View{nullptr}, OrbitCenter{0.0, 0.0, 0.0}, Sensitivity{1.0}, bFollowMouse{false} {}

ViewControllerMode ViewController::GetMode() const {return Mode;}

void ViewController::SetTargetView(View3D *View) {this->View = View;}

void ViewController::SetMode(ViewControllerMode Mode) {this->Mode = Mode;}

void ViewController::SetLastClick(double xpos, double ypos)
{
	LastClick.xpos = xpos;
	LastClick.ypos = ypos;
}

void ViewController::SetOrbitCenter(double x, double y, double z)
{
	OrbitCenter[0] = x;
	OrbitCenter[1] = y;
	OrbitCenter[2] = z;
}

void ViewController::MoveTo(double xpos, double ypos) const
{
	switch (Mode) {
		case (ORBIT):
			printf("Orbit MoveTo %lf %lf\n", xpos, ypos);
			break;
		case (TURNTABLE):
			break;
		case (FLY):
			break;
		case (WALK):
			break;
		case (PAN):
			printf("Pan MoveTo %lf %lf\n", xpos, ypos);
			break;
		case (ZOOM):
			printf("Zoom MoveTo %lf %lf\n", xpos, ypos);
			break;
	}
}

void ViewController::ScrollBy(double xoffset, double yoffset) const
{
	printf("ScrollBy %lf %lf\n", xoffset, yoffset);
			
}



