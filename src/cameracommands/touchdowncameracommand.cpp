#include <algorithm>
#ifdef APL
#include <cmath>
#endif

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMProcessing.h>

#include "cameracommands/touchdowncameracommand.h"
#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

TouchdownCameraCommand::TouchdownCameraCommand()
{
    mVerticalSpeedDataRef = XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm");
    mOnGroundDataRef = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    mRadioAltDataRef = XPLMFindDataRef("sim/flightmodel/position/y_agl");
    mVspeed = 0;
    mTouchdownTime = 0;
    mTouchdown = true;
    mPrevOnGround = true;
    mResponse = 25;
    mLastPitch = 0;
    mLastRoll = 0;
    mLastX = 0;
}

TouchdownCameraCommand::~TouchdownCameraCommand()
{
    //dtor
}

void TouchdownCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    float currentTime, touchdownshake, fastshake;
    const double PI = 3.141592653589793238463;

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

    // Reset if not on ground
    if (!XPLMGetDatai(mOnGroundDataRef)) {
        // Reset only after climbing 1m agl
        if (XPLMGetDataf(mRadioAltDataRef) > 1)
            mPrevOnGround = false;
    }

    // Touchdown effect
    if (!mPrevOnGround && XPLMGetDatai(mOnGroundDataRef)) {
        // At touchdown register the time
        mPrevOnGround = true;
        mTouchdownTime = XPLMGetElapsedTime();
        mVspeed = XPLMGetDataf(mVerticalSpeedDataRef);
        // The vspeed defines the power between 0 and 1
        // Maximum vspeed in -500 ft/min
        mVspeed = std::abs(mVspeed);
        mVspeed = std::max(100.0f, std::min(500.0f, mVspeed)) / 500.0f;
    } else {
        // If we are in the period between touchdown an touchdown + 1.5s
        // animate the touchdown effect
        currentTime = XPLMGetElapsedTime() - mTouchdownTime;
        if (currentTime > 0 && currentTime < 1.5f) {
            touchdownshake = (mVspeed * mResponse / 100.0f) * (std::sin(currentTime * PI * 7.0f) / (currentTime * 50.0f));
            fastshake = (mVspeed * mResponse / 100.0f) * (std::sin(currentTime * 50.0f) / (currentTime * 50.0f));
            mLastX = fastshake * 0.01f;
            mLastRoll = fastshake * 15.0f;
            mLastPitch = touchdownshake * 20.0f;
        }
    }
    position.pitch += mLastPitch;
    position.roll += mLastRoll;
    position.x += mLastX;
}

void TouchdownCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}

void TouchdownCameraCommand::set_response(float response)
{
    mResponse = response;
}

float TouchdownCameraCommand::get_response()
{
    return mResponse;
}

float TouchdownCameraCommand::get_last_roll()
{
  return mLastRoll;
}
