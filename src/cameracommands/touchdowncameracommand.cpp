#include <algorithm>
#ifdef APL
#include <cmath>
#endif

#include <string>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/XPLM/XPLMPlugin.h>

#include "cameracommands/touchdowncameracommand.h"
#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

#define MSG_ADD_DATAREF 0x01000000

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

    // Nose bump
    mNoseWheelPrevOnGround = true;
    noseWheelOnGround[0] = 1;
    mGearsOnGroundDataRef = XPLMFindDataRef("sim/flightmodel2/gear/on_ground");
    mLastY = 0;
    noseWheelTouchdownTime = 0;
    bumpInitialAmplitude = 0.03f;
    bumpDecayRate = 1.0f;
    bumpFrequency = 1.0f;
}

TouchdownCameraCommand::~TouchdownCameraCommand()
{
    //dtor
}

void TouchdownCameraCommand::on_enable()
{
    int datarefEditorId = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");

    mBumpInitialAmplitude = XPLMRegisterDataAccessor(
        "simcoders/headshake/touchdowncamera/bump_initial_amplitude", xplmType_Float, 1, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpInitialAmplitude;
            return 0.01f;
        }, [](void* refCon, float value) -> void {
            if (refCon) {
                reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpInitialAmplitude = value;
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/touchdowncamera/bump_initial_amplitude");

    mBumpDecayRate = XPLMRegisterDataAccessor(
        "simcoders/headshake/touchdowncamera/bump_decay_rate", xplmType_Float, 1, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpDecayRate;
            return 0.01f;
        }, [](void* refCon, float value) -> void {
            if (refCon) {
                reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpDecayRate = value;
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/touchdowncamera/bump_decay_rate");

    mBumpFrequency = XPLMRegisterDataAccessor(
        "simcoders/headshake/touchdowncamera/bump_frequency", xplmType_Float, 1, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpFrequency;
            return 0.01f;
        }, [](void* refCon, float value) -> void {
            if (refCon) {
                reinterpret_cast<TouchdownCameraCommand*>(refCon)-> bumpFrequency = value;
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/touchdowncamera/bump_frequency");
}

void TouchdownCameraCommand::on_disable()
{
    XPLMUnregisterDataAccessor(mBumpInitialAmplitude);
    XPLMUnregisterDataAccessor(mBumpDecayRate);
    XPLMUnregisterDataAccessor(mBumpFrequency);
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
    position.y -= mLastY;

    // Reset the state vars
    mLastPitch = 0;
    mLastRoll = 0;
    mLastX = 0;
    
    mLastY = 0;

    // Exit if disabled
    if (!pEnabled)
        return;

    // Reset if not on ground
    if (!XPLMGetDatai(mOnGroundDataRef)) {
        // Reset only after climbing 1m agl
        if (XPLMGetDataf(mRadioAltDataRef) > 1) {
            mPrevOnGround = false;
            mNoseWheelPrevOnGround = false;
        }            
    }

    // Bump effect
    XPLMGetDatavi(mGearsOnGroundDataRef, noseWheelOnGround, 0, 1); // use better way to detect the nose wheel -> sim/aircraft/parts/acf_gear_ynodef
    if(!mNoseWheelPrevOnGround && noseWheelOnGround[0] == 1) {
        XPLMDebugString("Bump Effect - Nose wheel touched down.\n");
        mNoseWheelPrevOnGround = true;
        noseWheelTouchdownTime = XPLMGetElapsedTime();
    } else {        
        float timeSinceNoseWheelTouchdown = XPLMGetElapsedTime() - noseWheelTouchdownTime;
        if (timeSinceNoseWheelTouchdown > 0 && timeSinceNoseWheelTouchdown < 1.5f) {
            mLastY = bumpInitialAmplitude * exp(-bumpDecayRate * timeSinceNoseWheelTouchdown) * std::sin(2 * PI * bumpFrequency * timeSinceNoseWheelTouchdown);
            std::string s = "Bump Effect - Time since touch down: " + std::to_string(timeSinceNoseWheelTouchdown) +  " Y offset = " + std::to_string(mLastY) + "\n";
            XPLMDebugString(s.c_str());
        }
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
    position.y += mLastY;
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
