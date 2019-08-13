#ifndef CAMERAPOSITION_H
#define CAMERAPOSITION_H

#include <stdio.h>
#include <math.h>

#include "helpers.h"

//#define EPSILON 1e-12
#define EPSILON (1.0f/(float)(1<<FRAC_BIT_RES))

struct CameraPosition
{
public:
    float pitch;
    float roll;
    float yaw;
    float x;
    float y;
    float z;

    CameraPosition()
    {
        pitch = 0;
        roll = 0;
        yaw = 0;
        x = 0;
        y =0;
        z = 0;
    }

    CameraPosition operator-(const CameraPosition &other)
    {
        CameraPosition pos;

        pos.x = x - other.x;
        pos.y = y - other.y;
        pos.z = z - other.z;
        pos.yaw = yaw - other.yaw;
        pos.pitch = pitch - other.pitch;
        pos.roll = roll - other.roll;

        return pos;
    }

    CameraPosition operator+(const CameraPosition &other)
    {
        CameraPosition pos;

        pos.x = x + other.x;
        pos.y = y + other.y;
        pos.z = z + other.z;
        pos.yaw = yaw + other.yaw;
        pos.pitch = pitch + other.pitch;
        pos.roll = roll + other.roll;

        return pos;
    }

    bool operator==(const CameraPosition& other)
    {
	float rollDiff = fabs(roll - other.roll);
	if (rollDiff > 180.0f)
	{
		rollDiff = 360.0f - rollDiff;
	}
	float pitchDiff = fabs(pitch - other.pitch);
	if (pitchDiff > 180.0f)
	{
		pitchDiff = 360.0 - pitchDiff;
	}
	float yawDiff = fabs(yaw - other.yaw);
	if (yawDiff > 180.0f)
	{
		yawDiff = 360.0f - yawDiff;
	}
        return  rollDiff  < EPSILON &&
                pitchDiff < EPSILON &&
                yawDiff   < EPSILON &&
                fabs(x - other.x) < EPSILON &&
                fabs(y - other.y) < EPSILON &&
                fabs(z - other.z) < EPSILON;
    };

    bool operator!=(const CameraPosition& other)
    {
        return !(*this == other);
    };

    char* to_string()
    {
        char* buf = new char[1024];
        sprintf(buf, "pitch=%f roll=%f yaw=%f x=%f y=%f z=%f\n", pitch, roll, yaw, x, y, z);
        return buf;
    }

  // normalize - adjust roll, pitch and yaw so they are ensured to be within standard limits
  void normalize()
  {
    roll  = fmodf(roll + 180.0f, 360.0f) - 180.0f;
    pitch = fmodf(pitch + 180.0f, 360.0f) - 180.0f;
    yaw   = fmodf(yaw + 360.0f, 360.f);
  }
  
protected:
private:
};

#endif // CAMERAPOSITION_H
