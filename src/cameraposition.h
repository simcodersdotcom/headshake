#ifndef CAMERAPOSITION_H
#define CAMERAPOSITION_H

#include <stdio.h>
#include <math.h>

#define EPSILON 1e-12

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
        return fabs(roll - other.roll) < EPSILON &&
                fabs(pitch - other.pitch) < EPSILON &&
                fabs(yaw - other.yaw) < EPSILON &&
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
protected:
private:
};

#endif // CAMERAPOSITION_H
