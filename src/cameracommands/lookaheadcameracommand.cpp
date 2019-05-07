#include <math.h>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"
#include "cameracommands/lookaheadcameracommand.h"

LookAheadCameraCommand::LookAheadCameraCommand()
{
    // Setup the datarefs
    mGNormalDataRef = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
    mDamper = 5;
    mResponse = 45;
    mLastAlpha = 0;
    mTargetAlpha = 0;
    mLastPitch = 0;
}

LookAheadCameraCommand::~LookAheadCameraCommand()
{
    //dtor
}

void LookAheadCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    float acc, currentG;

    position.pitch -= mLastPitch;
    // Reset the state vars
    mLastPitch = 0;

    // Exit if disabled
    if (!pEnabled)
        return;

    // Pitch
    currentG = XPLMGetDataf(mGNormalDataRef);
    // Work only for positive Gs
    if (currentG > 0) {
        mPitchFilter.insert(mPitchFilter.begin(), currentG - 1);
        while (mPitchFilter.size() > mDamper)
            mPitchFilter.pop_back();
        acc = average(mPitchFilter);
        if (acc > 4.0f)
            mTargetAlpha = 1.0f;
        else if ((mTargetAlpha < 0.5f && acc > 2.0f) || (mTargetAlpha > 0.8f && acc < 3.4f))
            mTargetAlpha = 0.5f;
        else if (mTargetAlpha > 0.4f && acc < 1.5f)
            mTargetAlpha = 0;
    } else {
        mTargetAlpha = 0;
    }
    mLastAlpha += (mTargetAlpha - mLastAlpha) / 20.0f;
    mLastPitch = mLastAlpha * mResponse;
    position.pitch += mLastPitch;
}

void LookAheadCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}

void LookAheadCameraCommand::set_response(float response)
{
    mResponse = response;
}

float LookAheadCameraCommand::get_response()
{
    return mResponse;
}

// Executed when the view type changes
void LookAheadCameraCommand::on_view_changed(int viewCode)
{
    // If the user enters the virtualcokpit, reset the data
    if (viewCode == 1026) {
        mLastAlpha = 0;
        mLastPitch = 0;
    }
}
