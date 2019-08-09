#include <algorithm>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"
#include "cameracommands/groundrollcameracommand.h"

GroundRollCameraCommand::GroundRollCameraCommand()
{
    // Setup the datarefs
    mGNormalDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    mOnGroundDataRef = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    mGSideDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_side");
    mBrakesDataRef = XPLMFindDataRef("sim/cockpit2/controls/parking_brake_ratio");
    mGSDataRef = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
    // Setup the private vars
    mResponse = 25;
    mLastPitch = 0;
    mLastRoll = 0;
    mLastX = 0;
    mLastNormG = 1;
}

GroundRollCameraCommand::~GroundRollCameraCommand()
{
    //dtor
}

void GroundRollCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    float acc, currentNormalG, currentSideG;

    // Restore the initial position
    position.pitch -= mLastPitch;
    position.roll -= mLastRoll;
    position.x -= mLastX;

    // Reset the state vars
    mLastPitch = 0;
    mLastRoll = 0;
    mLastX = 0;

    // Exit if disabled
    if (!pEnabled)
        return;

    if (XPLMGetDatai(mOnGroundDataRef) && XPLMGetDataf(mGSDataRef) > 1) {
        currentNormalG = std::max(0.92f, std::min(1.08f, XPLMGetDataf(mGNormalDataRef)));
        if (XPLMGetDataf(mBrakesDataRef) > 0) {
            currentNormalG = mLastNormG + (currentNormalG - mLastNormG) * 0.1;
        }
        mLastNormG = currentNormalG;
        currentSideG = std::max(-0.04f, std::min(0.04f, XPLMGetDataf(mGSideDataRef)));
    } else {
        currentNormalG = 1;
        currentSideG = 0;
    }

    // Pitch
    // Limit the variations between a minimum and a maximum
    mPitchFilter.insert(mPitchFilter.begin(), currentNormalG - 1);
    if (mPitchFilter.size() > 5)
        mPitchFilter.pop_back();
    acc = continue_log(average(mPitchFilter));
    mLastPitch = quantize(-(acc * mResponse / 10.0f));
    position.pitch += mLastPitch;

    // Roll
    mYawFilter.insert(mYawFilter.begin(), currentSideG);
    if (mYawFilter.size() > 25)
        mYawFilter.pop_back();
    acc = continue_log(average(mYawFilter));
    mLastRoll -= quantize((acc * mResponse / 5.0f));
    position.roll += mLastRoll;

    // X
    // (works like the roll but with a different response)
    mLastX -= (acc * mResponse / 500.0f);
    position.x += mLastX;
}

void GroundRollCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}

void GroundRollCameraCommand::set_response(float response)
{
    mResponse = response;
}

float GroundRollCameraCommand::get_response()
{
    return mResponse;
}

float GroundRollCameraCommand::get_last_roll()
{
  return mLastRoll;
}

void GroundRollCameraCommand::reset_last_roll()
{
    mLastRoll = 0.0f;
}

// Executed when the view type changes
void GroundRollCameraCommand::on_view_changed(int viewCode)
{
    // If the user enters the virtualcokpit, reset the data
    if (viewCode == 1026) {
      this->reset_last_roll();
    }
}
