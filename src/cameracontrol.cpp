#include <stddef.h>  // defines NULL
#include <vector>
#include <algorithm>
#ifdef APL
#include <cmath>
#endif

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMPlugin.h>

#include "cameracontrol.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"
#include "cameracommands/gforcecameracommand.h"
#include "cameracommands/lookaheadcameracommand.h"
#include "cameracommands/groundrollcameracommand.h"
#include "cameracommands/taxilookaheadcameracommand.h"
#include "cameracommands/guncameracommand.h"
#include "cameracommands/pistonenginecameracommand.h"
#include "cameracommands/rotorcameracommand.h"
#include "cameracommands/touchdowncameracommand.h"

#define MSG_ADD_DATAREF 0x01000000

CameraControl* CameraControl::mInstance = NULL;
CameraControl* CameraControl::get_instance()
{
    if (!mInstance)
        mInstance = new CameraControl;
    return mInstance;
}

CameraControl::CameraControl()
{
    // Instance the new commands
    mCommands.push_back(new GForceCameraCommand);
    mCommands.push_back(new LookAheadCameraCommand);
    mCommands.push_back(new PistonEngineCameraCommand);
    mCommands.push_back(new RotorCameraCommand);
    mCommands.push_back(new GroundRollCameraCommand);
    mCommands.push_back(new TaxiLookAheadCameraCommand);
    mCommands.push_back(new GunCameraCommand);
    mCommands.push_back(new TouchdownCameraCommand);
    // Store the size in a private property
    // to make loops more efficient.
    // An iterator is not needed because the size of this vector
    // will stay the same until the plugin is stopped.
    mCommandsSize = mCommands.size();
    mFreezed1 = false;
    mFreezed2 = false;
    mError = false;
    mMultimonitorCompatibility = false;
    mPositionInited = false;
    mOverride = false;
    mLastCameraType = 0;
    mLastJoyPitch = 0;
    mLastJoyYaw = 0;
    mCameraOffset.x = mCameraOffset.y = mCameraOffset.z = mCameraOffset.roll = mCameraOffset.pitch = mCameraOffset.yaw = 0;
    // Fill the datarefs
    mCinemaVeriteDataRef = XPLMFindDataRef("sim/graphics/view/cinema_verite");
    mPausedDataRef = XPLMFindDataRef("sim/time/paused");
    mViewTypeDataRef = XPLMFindDataRef("sim/graphics/view/view_type");
    mHeadHeadingDataRef = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    mHeadPitchDataRef = XPLMFindDataRef("sim/graphics/view/pilots_head_the");
    mHeadRollDataRef = XPLMFindDataRef("sim/graphics/view/field_of_view_roll_deg");
    mHeadXDataRef = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    mHeadYDataRef = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    mHeadZDataRef = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    mJoyAxisAssignments = XPLMFindDataRef("sim/joystick/joystick_axis_assignments");
    mJoyAxisValues = XPLMFindDataRef("sim/joystick/joystick_axis_values");
    // Fill the commands
    // These are the commands that stop the plugin for a while when called
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_0"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_1"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_2"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_3"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_4"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_5"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_6"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_7"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_8"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_9"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_10"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_11"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_12"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_13"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_14"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_15"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_16"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_17"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_18"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/quick_look_19"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/3d_cockpit_cmnd_look"));
    mStopCommands.push_back(XPLMFindCommand("sim/view/3d_cockpit_toggle"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/left"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/right"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/up"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/down"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/forward"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/backward"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/left_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/right_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/up_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/down_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/forward_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/backward_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/left_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/right_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/up_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/down_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/forward_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/backward_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_left"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_right"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_up"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_down"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_left_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_right_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_up_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_down_fast"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_left_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_right_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_up_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/rot_down_slow"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_up"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_up_right"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_right"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_down_right"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_down"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_down_left"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_left"));
    mStopCommands.push_back(XPLMFindCommand("sim/general/hat_switch_up_left"));

    mStopCommandsSize = mStopCommands.size();
}

CameraControl::~CameraControl()
{
    // Delete the commands and free the memory
    for (unsigned int i = 0; i < mCommandsSize; i++) {
        if (mCommands.at(i) != NULL) {
            delete mCommands.at(i);
        }
    }
    mCommands.clear();
    // Delete the stopcommands and free the memory
    mStopCommands.clear();
}

void CameraControl::on_enable()
{
    int datarefEditorId = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");
    XPLMDataRef dref;
    mPositionInited = false;

    // Publish the override and offset datarefs
    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/override", xplmType_Int, 1,
        [](void* refCon) -> int {
            if (refCon) return ((CameraControl*)refCon)->get_override() ? 1 : 0;
            return 0;
        }, [](void* refCon, int value) -> void {
            if (refCon) {
                ((CameraControl*)refCon)->set_override(value == 1);
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    mDrefs.push_back(dref);
    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_x", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().x;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);
    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_y", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().y;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);
    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_z", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().z;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);

    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_yaw", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().yaw;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);

    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_pitch", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().pitch;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);

    dref = XPLMRegisterDataAccessor(
        "simcoders/headshake/offset_roll", xplmType_Float, 0, NULL, NULL,
        [](void* refCon) -> float {
            if (refCon) return ((CameraControl*)refCon)->get_offset().roll;
            return 0.0f;
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
    mDrefs.push_back(dref);
    // Register the datarefs in the dataref editor
    if (datarefEditorId != XPLM_NO_PLUGIN_ID){
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/override");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_x");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_y");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_z");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_yaw");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_pitch");
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/offset_roll");
    }

    // Register the commands
    for (unsigned int i = 0; i < mStopCommandsSize; i++) {
        if (mStopCommands.at(i) != NULL) {
            XPLMRegisterCommandHandler(mStopCommands.at(i), [](XPLMCommandRef, XPLMCommandPhase, void*) -> int {
                  CameraControl::mInstance->freeze();
                  return 1;
            }, true, 0);
        }
    }

    // Send the on_enable to the commands
    for (unsigned int i = 0; i < mCommandsSize; i++) {
        mCommands.at(i)->on_enable();
    }
}

void CameraControl::on_disable()
{
    // Unpublish the offset datarefs
    for (unsigned int i = 0; i < mDrefs.size(); i++) {
        XPLMUnregisterDataAccessor(mDrefs.at(i));
    }
    mDrefs.clear();
    // Restore the initial camera position
    if (mPositionInited && !mOverride) {
        XPLMSetDataf(mHeadPitchDataRef, mInitialPos.pitch);
        XPLMSetDataf(mHeadHeadingDataRef, mInitialPos.yaw);
        XPLMSetDataf(mHeadRollDataRef, mInitialPos.roll);
        XPLMSetDataf(mHeadXDataRef, mInitialPos.x);
        XPLMSetDataf(mHeadYDataRef, mInitialPos.y);
        XPLMSetDataf(mHeadZDataRef, mInitialPos.z);
    }
    // Unregister the commands
    for (unsigned int i = 0; i < mStopCommandsSize; i++) {
        if (mStopCommands.at(i) != NULL) {
            XPLMUnregisterCommandHandler(mStopCommands.at(i), [](XPLMCommandRef, XPLMCommandPhase, void*) -> int {
                  CameraControl::mInstance->freeze();
                  return 1;
            }, true, 0);
        }
    }
    // Send the on_enable to the commands
    for (unsigned int i = 0; i < mCommandsSize; i++) {
        mCommands.at(i)->on_disable();
    }
}

void CameraControl::freeze()
{
    mFreezed1 = true;
    mFreezed2 = true;
}

// Control the camera calling all the active modules
float CameraControl::control()
{
    CameraPosition currentPos, calculatedPos;
    int cameraType = XPLMGetDatai(mViewTypeDataRef);

    // If the camera type is changed, notify the objects passing the new camera type
    if (cameraType != mLastCameraType) {
        mLastCameraType = cameraType;
        for (unsigned int i = 0; i < mCommandsSize; i++) {
            mCommands.at(i)->on_view_changed(cameraType);
        }
    }
    // Make sure that we're in the virtual cockpit, cinema verite is not active and the sim is not paused
    mError = XPLMGetDatai(mCinemaVeriteDataRef) > 0;
    if (cameraType != 1026 || XPLMGetDatai(mPausedDataRef) || mError) {
        return 1;
    }

    // At first I tought to use two private methods:
    // - CameraPosition get_camera_position()
    // - void set_camera_position(CameraPosition currentPos)
    //
    // That's really inefficient because:
    // - those functions were called only once and only here
    // - every time this method is called (and it's called in a virtually infinite loop)
    //   calling those methods would swap the current scope

    // If not overridden, read the current camera position
    if (!mOverride) {
        currentPos.pitch = XPLMGetDataf(mHeadPitchDataRef);
        currentPos.yaw = XPLMGetDataf(mHeadHeadingDataRef);
        currentPos.roll = XPLMGetDataf(mHeadRollDataRef);
        currentPos.x = XPLMGetDataf(mHeadXDataRef);
        currentPos.y = XPLMGetDataf(mHeadYDataRef);
        currentPos.z = XPLMGetDataf(mHeadZDataRef);
    }

    if (!mPositionInited && (std::abs(currentPos.x) > 0 || std::abs(currentPos.y) > 0 || std::abs(currentPos.y) > 0)) {
        // Save the initial camera position
        mInitialPos = currentPos;
        mPositionInited = true;
    }

    // If the current position has changed since the last time we set it,
    // start monitoring it and wait for it to set
    if (mFreezed2) {
        if (!mFreezed1 && currentPos == mLastPos)
            mFreezed2 = false;
        mFreezed1 = false;
        mLastPos = currentPos;
        return -2;
    }

    for (unsigned int i = 0; i < mCommandsSize; i++) {
        mCommands.at(i)->execute(calculatedPos);
    }

    // If not overridden, update the camera position
    if (!mOverride) {
        // Detect the joystick axis used for camera movements
        float yawValue = 0, pitchValue = 0;
        int values[100];
        XPLMGetDatavi(mJoyAxisAssignments, values, 0, 100);
        for (unsigned int i = 0; i < 100; i++) {
            if (values[i] == 41) {
                XPLMGetDatavf(mJoyAxisValues, &yawValue, i, 1);
            }
            else if (values[i] == 42) {
                XPLMGetDatavf(mJoyAxisValues, &pitchValue, i, 1);
            }
        }

        if (yawValue - mLastJoyYaw < -0.001 || yawValue - mLastJoyYaw > 0.001 || pitchValue - mLastJoyPitch < -0.001 || pitchValue - mLastJoyPitch > 0.001) {
            mLastJoyPitch = pitchValue;
            mLastJoyYaw = yawValue;
            return -1;
        }

        mLastJoyPitch = pitchValue;
        mLastJoyYaw = yawValue;

        currentPos = currentPos + calculatedPos;
        // Set the current camera position
        XPLMSetDataf(mHeadPitchDataRef, std::max(std::min(currentPos.pitch, 89.0f), -89.0f)); // Limit the pitch to -89°/+89°
        XPLMSetDataf(mHeadHeadingDataRef, currentPos.yaw);
        // Do not write the roll if the multimonitor compatibility is turned on
        if (!mMultimonitorCompatibility)
            XPLMSetDataf(mHeadRollDataRef, currentPos.roll);
        XPLMSetDataf(mHeadXDataRef, currentPos.x);
        XPLMSetDataf(mHeadYDataRef, currentPos.y);
        XPLMSetDataf(mHeadZDataRef, currentPos.z);
    }
    // Set the position offset
    mCameraOffset = mCameraOffset + calculatedPos;
    // Save the updated position
    mLastPos = currentPos;

    return -1;
}

CameraPosition CameraControl::get_offset()
{
    return mCameraOffset;
}

void CameraControl::set_override(bool value)
{
    char buff[16];
    sprintf(buff, "set: %d\n", value);
    XPLMDebugString(buff);
    mOverride = value;
}

bool CameraControl::get_override()
{
    return mOverride;
}

void CameraControl::accept(IVisitor &visitor)
{
    visitor.visit(*this);
    for (unsigned int i = 0; i < mCommandsSize; i++)
        mCommands.at(i)->accept(visitor);
}

bool CameraControl::error()
{
    return mError;
}

bool CameraControl::get_multimonitor_compatibility()
{
    return mMultimonitorCompatibility;
}

void CameraControl::set_multimonitor_compatibility(bool compatibility)
{
    mMultimonitorCompatibility = compatibility;
}
