#include <algorithm>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"
#include "cameracommands/taxilookaheadcameracommand.h"

TaxiLookAheadCameraCommand::TaxiLookAheadCameraCommand()
{
    // Setup the datarefs
    mOnGroundDataRef    = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    mTurnDataRef        = XPLMFindDataRef("sim/flightmodel/position/R");
    mRudderDataRef      = XPLMFindDataRef("sim/joystick/yoke_heading_ratio");
    mGroundSpeedDataRef = XPLMFindDataRef("sim/flightmodel/position/groundspeed");

    // Setup the private vars
    mRudderResponse     = 25;
    mLeanResponse       = 25;
    mTurnResponse       = 25;
    mLastY              = 0;
    mLastZ              = 0;
    mLastYaw            = 0;
    mLastRoll           = 0;
}

TaxiLookAheadCameraCommand::~TaxiLookAheadCameraCommand()
{
    //dtor
}

void TaxiLookAheadCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    (void)position;

    float blend, acc, lean, leanInput, rudderInput, turnInput, groundSpeedAcc, currentTurn, currentRudder, currentGroundSpeed;

    // Restore the initial yaw & roll
    position.y     -= mLastY;
    position.z     -= mLastZ;
    position.yaw   -= mLastYaw;
    position.roll  -= mLastRoll;

    // Exit if disabled
    if (!pEnabled)
    {
        // Reset the state vars
        mLastY          = 0;
        mLastZ          = 0;
        mLastYaw        = 0;
        mLastRoll       = 0;
        return;
    }

    if (XPLMGetDatai(mOnGroundDataRef)) {
        currentTurn             = XPLMGetDataf(mTurnDataRef);
        currentRudder           = XPLMGetDataf(mRudderDataRef);
        currentGroundSpeed      = XPLMGetDataf(mGroundSpeedDataRef);
    } else {
        currentTurn             = 0;
        currentRudder           = 0;
        currentGroundSpeed      = 0;
    }

    // Rudder & lean input
    mRudderFilter.insert(mRudderFilter.begin(), currentRudder);
    if( mRudderFilter.size() > 30 )
        mRudderFilter.pop_back();
    rudderInput = average(mRudderFilter) * ( mRudderResponse / 100.0f ) * 45.0f;
    leanInput   = average(mRudderFilter) * ( mLeanResponse / 100.0f ) * 0.75f;

    // Turn
    mTurnFilter.insert(mTurnFilter.begin(), currentTurn);
    if( mTurnFilter.size() > 30 )
        mTurnFilter.pop_back();
    turnInput = average(mTurnFilter) * ( mTurnResponse / 25.0f );

    // Ground speed
    mGroundSpeedFilter.insert(mGroundSpeedFilter.begin(), currentGroundSpeed);
    if( mGroundSpeedFilter.size() > 10 )
        mGroundSpeedFilter.pop_back();
    groundSpeedAcc = average(mGroundSpeedFilter);

    // Blend the effect out based on the ground speed (max of 20m/sec)
    blend = ( 1.0f - std::max( 0.0f, std::min( groundSpeedAcc / 20.0f, 1.0f ) ) );
    
    // Mix the turn & rudder effects
    acc  = ( turnInput + rudderInput ) * 0.5f * blend;
    
    // Lean forwards (and to some degree upwards/off the seat)
    lean = ( leanInput * leanInput ) * blend;

    // Cache y, z, yaw & roll
    mLastY         = lean * 0.25f;
    mLastZ         = -lean;
    mLastYaw       = quantize(acc);
    mLastRoll      = quantize(acc * 0.125f);

    position.y    += mLastY;
    position.z    += mLastZ;
    position.yaw  += mLastYaw;
    position.roll += mLastRoll;
}

void TaxiLookAheadCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}

void TaxiLookAheadCameraCommand::set_rudder_response(float response)
{
    mRudderResponse = response;
}

float TaxiLookAheadCameraCommand::get_rudder_response() const
{
    return mRudderResponse;
}

void TaxiLookAheadCameraCommand::set_turn_response(float response)
{
    mTurnResponse = response;
}

float TaxiLookAheadCameraCommand::get_turn_response() const
{
    return mTurnResponse;
}

void TaxiLookAheadCameraCommand::set_lean_response(float response)
{
    mLeanResponse = response;
}

float TaxiLookAheadCameraCommand::get_lean_response() const
{
    return mLeanResponse;
}

float TaxiLookAheadCameraCommand::get_last_roll()
{
  return mLastRoll;
}

void TaxiLookAheadCameraCommand::reset_last_roll()
{
    mLastRoll = 0.0f;
}

// Executed when the view type changes
void TaxiLookAheadCameraCommand::on_view_changed(int viewCode)
{
    // If the user enters the virtualcokpit, reset the data
    if (viewCode == 1026) {
      this->reset_last_roll();
    }
}
