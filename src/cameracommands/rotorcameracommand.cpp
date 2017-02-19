#include <cmath>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "cameracommands/rotorcameracommand.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

RotorCameraCommand::RotorCameraCommand()
{
    // Setup the datarefs
    mRotorRpmDataRef = XPLMFindDataRef("sim/flightmodel2/engines/prop_rotation_speed_rad_sec");
    mRotorAngleDataRef = XPLMFindDataRef("sim/flightmodel2/engines/prop_rotation_angle_deg");
    mPropTypeDataRef = XPLMFindDataRef("sim/aircraft/prop/acf_prop_type");
    mNumBladesDataRef = XPLMFindDataRef("sim/aircraft/prop/acf_num_blades");
    mPropMassDataRef = XPLMFindDataRef("sim/aircraft/prop/acf_prop_mass");
    mOnGroundDataRef = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    // Define the default values
    mResponse = 25;
    mLastYaw = 0;
}

RotorCameraCommand::~RotorCameraCommand()
{

}

void RotorCameraCommand::execute(CameraPosition &position)
{
    const double PI = 3.141592653589793238463;
    float rotorRpmOutVal[8], rotorAngleOutVal[8], numBladesOutVal[8], propMassOutVal[8];
    int propTypeOutVal[8], count = 0;
    float shake = 0, angle, rpm, power;

    position.x -= mLastYaw;
    position.y -= 0.1 * mLastYaw;
    mLastYaw = 0;
    if (!pEnabled)
        return;

    // Read the values
    XPLMGetDatavf(mRotorRpmDataRef, rotorRpmOutVal, 0, 7);
    XPLMGetDatavf(mRotorAngleDataRef, rotorAngleOutVal, 0, 7);
    XPLMGetDatavf(mPropMassDataRef, propMassOutVal, 0, 7);
    XPLMGetDatavi(mPropTypeDataRef, propTypeOutVal, 0, 7);
    XPLMGetDatavf(mNumBladesDataRef, numBladesOutVal, 0, 7);
    // Use half power if the heli is in flight
    if (XPLMGetDatai(mOnGroundDataRef) == 0)
        power = 0.00050;
    else
        power = 0.00075;

    // Detect the angle and rpm of each engine and shake!
    for (int i = 0; i < 8; i++) {
        // Convert speed to rpms
        rpm = (rotorRpmOutVal[i] * 180 / PI); // deg/s
        rpm = (rpm / 360) * 60; // rpms
        // Only rotating rotors with mass (as some helicopters as no mass rotors!)
        if (propTypeOutVal[i] == 3 && rpm > 1 && propMassOutVal[i] > 0) {
            if (rpm > 250) {
                // High rpms
                // shake it basing on the angle which varies at the rotor frequency
                shake += power * (mResponse / 50.0f) * sin(XPLMGetElapsedTime() * 25 * numBladesOutVal[i]);
            } else if (rpm > 120) {
                // Medium rpms
                // shake it basing on the angle which varies at the rotor frequency
                shake += (power / 1.5f) * (mResponse / 50.0f) * sin(XPLMGetElapsedTime() * 18 * numBladesOutVal[i]);
            } else {
                // Low rpms
                angle = rotorAngleOutVal[i] * numBladesOutVal[i];
                // Convert to radians
                angle = angle * PI / 180.0f;
                shake += (power / 3) * (mResponse / 50.0f) * sin(angle);
            }
            count++;
        }
    }
    if (count > 0) {
        mLastYaw = shake / (float)count;
        position.x += mLastYaw;
        position.y += 0.1 * mLastYaw;
    }
}

void RotorCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}
