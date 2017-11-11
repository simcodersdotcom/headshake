#include <math.h>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "cameracommands/gforcecameracommand.h"
#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

GForceCameraCommand::GForceCameraCommand()
{
    // Setup the datarefs
    mGNormalDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    mGAxialDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
    mGSideDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_side");
    mRadioAltDataRef = XPLMFindDataRef("sim/flightmodel/position/y_agl");
    // Setup the private vars
    mDamper = 5;
    mZResponse = 25;
    mYawResponse = 25;
    mPitchResponse = 25;
    mLastZ = 0;
    mLastX = 0;
    mLastYaw = 0;
    mLastPitch = 0;
    mLastRoll = 0;
}

GForceCameraCommand::~GForceCameraCommand()
{

}

void GForceCameraCommand::execute(CameraPosition &position)
{
    const float maxAlt = 20, minAlt = 1; // Meters
    const unsigned int maxDamper = 30, minDamper = 5;
    float acc, currentG, aglAlt;

    // Restore the initial position
    position.z -= mLastZ;
    position.y -= -mLastZ / 2.0f;
    position.x -= mLastX;
    position.yaw -= mLastYaw;
    position.pitch -= mLastPitch;
    position.roll -= mLastRoll;

    // Reset the state vars
    mLastZ = 0;
    mLastX = 0;
    mLastYaw = 0;
    mLastPitch = 0;
    mLastRoll = 0;

    // Exit if disabled
    if (!pEnabled)
        return;

    // Calculate the damper basing on the altitude
    aglAlt = XPLMGetDataf(mRadioAltDataRef);
    if (aglAlt > maxAlt)
        mDamper = minDamper;
    else if (aglAlt > minAlt)
        mDamper = minDamper + (maxDamper - minDamper) * (maxAlt - aglAlt) / (maxAlt - minAlt);
    else
        mDamper = maxDamper;

    // Push backward/forward (axial)
    currentG = XPLMGetDataf(mGAxialDataRef);
    mZFilter.insert(mZFilter.begin(), currentG);
    if (mZFilter.size() > 10)
        mZFilter.pop_back();
    acc = continue_log(average(mZFilter));
    mLastZ -= (acc * mZResponse / 500.0f);
    mLastPitch -= (acc * mZResponse / 7.5f);
    position.y += -mLastZ / 2.0f;
    position.z += mLastZ;

    // Heading
    currentG = XPLMGetDataf(mGSideDataRef);
    mYawFilter.insert(mYawFilter.begin(), currentG);
    while (mYawFilter.size() > mDamper)
        mYawFilter.pop_back();
    acc = continue_log(average(mYawFilter));
    mLastYaw -= (acc * mYawResponse / 15);
    position.yaw += mLastYaw;

    // Roll
    // (works like the heading but with a different response)
    if (acc < 0.005 || acc > 0.005) {
        mLastRoll -= (acc * mYawResponse / 5);
        position.roll += mLastRoll;
    } else {
        mLastRoll = 0;
        position.roll = 0;
    }

    // X
    // (works like the heading but with a different response)
    mLastX -= (acc * mYawResponse / 500);
    position.x += mLastX;

    // Pitch
    currentG = XPLMGetDataf(mGNormalDataRef);
    mPitchFilter.insert(mPitchFilter.begin(), currentG - 1);
    while (mPitchFilter.size() > mDamper)
        mPitchFilter.pop_back();
    acc = average(mPitchFilter);
    mLastPitch -= (continue_log(acc) * mPitchResponse / 10.0f);
    position.pitch += mLastPitch;
}

void GForceCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}

void GForceCameraCommand::set_pitch_response(float response)
{
    mPitchResponse = response;
}

float GForceCameraCommand::get_pitch_response()
{
    return mPitchResponse;
}

void GForceCameraCommand::set_acceleration_response(float response)
{
    mZResponse = response;
}

float GForceCameraCommand::get_acceleration_response()
{
    return mZResponse;
}

void GForceCameraCommand::set_yaw_response(float response)
{
    mYawResponse = response;
}

float GForceCameraCommand::get_yaw_response()
{
    return mYawResponse;
}
