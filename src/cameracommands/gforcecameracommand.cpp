#include <math.h>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/XPLM/XPLMPlugin.h>

#include "cameracommands/gforcecameracommand.h"
#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

#define MSG_ADD_DATAREF 0x01000000

GForceCameraCommand::GForceCameraCommand()
{
    // Setup the datarefs
    mGNormalDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    mGAxialDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
    mGSideDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_side");
    mRadioAltDataRef = XPLMFindDataRef("sim/flightmodel/position/y_agl");
    mOnGroundDataRef = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    mBrakesDataRef = XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");
    mGSDataRef = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
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
    mLastNormG = 1;
    mLastAxialG = 0;
    mGeneralSensitivity = 1;
}

GForceCameraCommand::~GForceCameraCommand()
{

}

void GForceCameraCommand::on_enable()
{
    int datarefEditorId = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");

    // Publish the drefs
    mGeneralSensitivityDataRef = XPLMRegisterDataAccessor(
        "simcoders/headshake/gforce/sensitivity", xplmType_Float, 1, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return reinterpret_cast<GForceCameraCommand*>(refCon)->mGeneralSensitivity;
            return 0;
        }, [](void* refCon, float value) -> void {
            if (refCon) {
                if (value < 0) {
                    value = 0;
                }
                if (value > 1) {
                    value = 1;
                }
                reinterpret_cast<GForceCameraCommand*>(refCon)->mGeneralSensitivity = value;
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/gforce/sensitivity");
}

void GForceCameraCommand::on_disable()
{
    XPLMUnregisterDataAccessor(mGeneralSensitivityDataRef);
}

void GForceCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    const float maxAlt = 20, minAlt = 1; // Meters
    const unsigned int maxDamper = 30, minDamper = 5;
    float acc, currentG, aglAlt;
    bool filter = XPLMGetDatai(mOnGroundDataRef) == 1 && XPLMGetDataf(mBrakesDataRef) > 0;
    float gs = XPLMGetDataf(mGSDataRef);

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
    if (!pEnabled) {
        return;
    }

    // Calculate the damper basing on the altitude
    aglAlt = XPLMGetDataf(mRadioAltDataRef);
    
    if (aglAlt > maxAlt) {
        mDamper = minDamper;
    } else if (aglAlt > minAlt) {
        mDamper = minDamper + (maxDamper - minDamper) * (maxAlt - aglAlt) / (maxAlt - minAlt);
    } else {
        mDamper = maxDamper;
    }

    // Push backward/forward (axial)
    currentG = gs > 1 ? XPLMGetDataf(mGAxialDataRef) : 0;
    if (filter) {
        currentG = mLastAxialG + (currentG - mLastAxialG) * 0.1;
    }
    mLastAxialG = currentG;
    mZFilter.insert(mZFilter.begin(), currentG);
    if (mZFilter.size() > 10) {
        mZFilter.pop_back();
    }
    acc = continue_log(average(mZFilter));
    mLastZ -= (acc * mZResponse * mGeneralSensitivity / 500.0f);
    mLastPitch -= quantize(acc * mZResponse * mGeneralSensitivity / 7.5f);
    position.y += -mLastZ / 2.0f;
    position.z += mLastZ;

    // Heading
    currentG = gs > 1 ? XPLMGetDataf(mGSideDataRef) : 0;
    mYawFilter.insert(mYawFilter.begin(), currentG);
    while (mYawFilter.size() > mDamper)
        mYawFilter.pop_back();
    acc = continue_log(average(mYawFilter));
    mLastYaw -= quantize(acc * mYawResponse * mGeneralSensitivity / 15);
    position.yaw += mLastYaw;

    // Roll
    // (works like the heading but with a different response)
    if (acc < -0.005 || acc > 0.005) {
        mLastRoll -= quantize((acc * mYawResponse * mGeneralSensitivity / 5));
        position.roll += mLastRoll;
    } else {
        mLastRoll = 0;
        //position.roll = 0;   // Shouldn't just set roll to zero, right?!?
    }

    // X
    // (works like the heading but with a different response)
    mLastX -= (acc * mYawResponse * mGeneralSensitivity / 500);
    position.x += mLastX;

    // Pitch
    currentG = gs > 1 ? XPLMGetDataf(mGNormalDataRef) : 1;
    if (filter) {
        currentG = mLastNormG + (currentG - mLastNormG) * 0.1;
    }
    mLastNormG = currentG;
    mPitchFilter.insert(mPitchFilter.begin(), currentG - 1);
    while (mPitchFilter.size() > mDamper)
        mPitchFilter.pop_back();
    acc = average(mPitchFilter);
    mLastPitch -= quantize(continue_log(acc) * mPitchResponse * mGeneralSensitivity / 10.0f);
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

float GForceCameraCommand::get_last_roll()
{
  return mLastRoll;
}

void GForceCameraCommand::reset_last_roll()
{
    mLastRoll = 0.0f;
}

// Executed when the view type changes
void GForceCameraCommand::on_view_changed(int viewCode)
{
    // If the user enters the virtualcokpit, reset the data
    if (viewCode == 1026) {
      this->reset_last_roll();
    }
}
