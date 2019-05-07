#include <vector>
#include <algorithm>
#include <cmath>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/XPLM/XPLMPlugin.h>

#include "cameracommands/pistonenginecameracommand.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"

#define MSG_ADD_DATAREF 0x01000000

PistonEngineCameraCommand::PistonEngineCameraCommand()
{
    // Load the dataref
    mNumEngineDataRef = XPLMFindDataRef("sim/aircraft/engine/acf_num_engines");
    mEngineTypeDataRef = XPLMFindDataRef("sim/aircraft/prop/acf_en_type");
    mEngineRotationDataRef = XPLMFindDataRef("sim/flightmodel2/engines/engine_rotation_angle_deg");
    mEngineRpmDataRef = XPLMFindDataRef("sim/cockpit2/engine/indicators/engine_speed_rpm");
    mOnGroundDataRef = XPLMFindDataRef("sim/flightmodel/failures/onground_any");
    // Set the starting shakes
    mLastXShake = 0;
    mLastYShake = 0;
    mResponse = 25;
    mOverride = false;

    mOverrideVibrationRatio[0] = 0;
    mOverrideVibrationRatio[1] = 0;
    mOverrideVibrationRatio[2] = 0;
    mOverrideVibrationRatio[3] = 0;
    mOverrideVibrationRatio[4] = 0;
    mOverrideVibrationRatio[5] = 0;
    mOverrideVibrationRatio[6] = 0;
    mOverrideVibrationRatio[7] = 0;

    mOverrideVibrationFreq[0] = 50;
    mOverrideVibrationFreq[1] = 50;
    mOverrideVibrationFreq[2] = 50;
    mOverrideVibrationFreq[3] = 50;
    mOverrideVibrationFreq[4] = 50;
    mOverrideVibrationFreq[5] = 50;
    mOverrideVibrationFreq[6] = 50;
    mOverrideVibrationFreq[7] = 50;
}

PistonEngineCameraCommand::~PistonEngineCameraCommand()
{

}

void PistonEngineCameraCommand::on_enable()
{
    int datarefEditorId = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");

    // Publish the drefs
    mOverrideDataref = XPLMRegisterDataAccessor(
        "simcoders/headshake/pistonengine/override_vibration", xplmType_Int, 1,
        [](void* refCon) -> int {
            if (refCon) return reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverride ? 1 : 0;
            return 0;
        }, [](void* refCon, int value) -> void {
            if (refCon) {
                reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverride = value == 1;
            }
        }, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, this);
    XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/pistonengine/override_vibration");

    mVibrationRatioDataref = XPLMRegisterDataAccessor(
        "simcoders/headshake/pistonengine/vibration_ratio", xplmType_FloatArray, 1,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        [](void *refCon, float *outValues, int inOffset, int inMax) -> int {
            if (outValues == NULL) {
                return 0;
            }
            if (refCon) {
                int i = 0;
                for (i = inOffset; (i < 8) && (i < inOffset+inMax); i++) {
                    outValues[i] = reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverrideVibrationRatio[i];
                }
                return i;
            }
            return 0;
        }, [](void *refCon, float *inValues, int inOffset, int inMax) -> void {
            if (inValues != NULL) {
                if (refCon) {
                    for (int i = inOffset; (i < 8) && (i < inOffset+inMax); i++) {
                        reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverrideVibrationRatio[i] = inValues[i];
                    }
                }
            }
        }, NULL, NULL, this, this);
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/pistonengine/vibration_ratio");

    mVibrationRatioDataref = XPLMRegisterDataAccessor(
        "simcoders/headshake/pistonengine/vibration_freq", xplmType_FloatArray, 1,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        [](void *refCon, float *outValues, int inOffset, int inMax) -> int {
            if (outValues == NULL) {
                return 0;
            }
            if (refCon) {
                int i = 0;
                for (i = inOffset; (i < 8) && (i < inOffset+inMax); i++) {
                    outValues[i] = reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverrideVibrationFreq[i];
                }
                return i;
            }
            return 0;
        }, [](void *refCon, float *inValues, int inOffset, int inMax) -> void {
            if (inValues != NULL) {
                if (refCon) {
                    for (int i = inOffset; (i < 8) && (i < inOffset+inMax); i++) {
                        reinterpret_cast<PistonEngineCameraCommand*>(refCon)->mOverrideVibrationFreq[i] = inValues[i];
                    }
                }
            }
        }, NULL, NULL, this, this);
        XPLMSendMessageToPlugin(datarefEditorId, MSG_ADD_DATAREF, (void*)"simcoders/headshake/pistonengine/vibration_freq");
}

void PistonEngineCameraCommand::on_disable()
{
    XPLMUnregisterDataAccessor(mOverrideDataref);
    XPLMUnregisterDataAccessor(mVibrationRatioDataref);
}

void PistonEngineCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
    CameraCommand::execute(position, elapsedTime);
    
    const double PI = 3.141592653589793238463;
    int engineTypeOutVal[8], engineNum;
    float engineRotationOutVal[8];
    float engineRpmOutVal[8];
    float power, xshake = 0, yshake=0, maxYShake = 0, maxXShake = 0, angle;

    // Reset the last position
    position.x -= mLastXShake;
    position.y -= mLastYShake;
    // Reset the last position vars
    mLastXShake = 0;
    mLastYShake = 0;
    // Return if not enabled
    if (!pEnabled)
        return;
    //Read the necessary vars
    engineNum = XPLMGetDatai(mNumEngineDataRef);
    XPLMGetDatavf(mEngineRotationDataRef, engineRotationOutVal, 0, 7);
    XPLMGetDatavf(mEngineRpmDataRef, engineRpmOutVal, 0, 7);
    XPLMGetDatavi(mEngineTypeDataRef, engineTypeOutVal, 0, 7);

    // Vary the power using the mResponse value;
    power = mResponse / 200.0f;
    // Half power if the plane is flying
    if (XPLMGetDatai(mOnGroundDataRef) == 0)
        power = power / 2.0f;

    // Detect the rpm of each engine and shake!
    for (int i = 0; i < engineNum; i++) {
        // Do this only on piston engines (injected or carb)
        if (engineTypeOutVal[i] == 1 || engineTypeOutVal[i] == 0) {
            if (mOverride) {

                // If overridden, use the ref data
                xshake = power * (mOverrideVibrationRatio[i] / 1000) * sin(mOverrideVibrationFreq[i] * XPLMGetElapsedTime());
                yshake = (power / 2) * (mOverrideVibrationRatio[i] / 1000) * sin((mOverrideVibrationFreq[i] + 10) * XPLMGetElapsedTime());

                if (std::abs(xshake) > std::abs(maxXShake))
                    maxXShake = xshake;
                if (std::abs(yshake) > std::abs(maxYShake))
                    maxYShake = yshake;

            } else {

                // Otherwise use the default data
                if (engineRpmOutVal[i] > 300) {

                    // High rpms

                    xshake = power / std::abs(engineRpmOutVal[i]) * sin(50 * XPLMGetElapsedTime());
                    xshake = (power / 2) / std::abs(engineRpmOutVal[i]) * sin(60 * XPLMGetElapsedTime());

                    if (std::abs(xshake) > std::abs(maxXShake))
                        maxXShake = xshake;
                    if (std::abs(yshake) > std::abs(maxYShake))
                        maxYShake = yshake;
                } else if (engineRpmOutVal[i] > 5) {

                    // Low rpms

                    // Continue if the cylinder is at low compression
                    // (more than 45° after each 90°)
                    if ((int)engineRotationOutVal[i] % 90 <= 30) {
                        angle = (float)((int)engineRotationOutVal[i] % 90);
                        // Convert the angle to 360°
                        angle = angle / 30.0f * 360.0f;
                        // Convert to radians
                        angle = angle * PI / 180.0f;
                        maxXShake = power * 0.005f * sin(angle);
                        maxYShake = maxXShake * 0.5f;
                    }
                }
            }
        }
    }
    mLastXShake = maxXShake;
    mLastYShake = maxYShake;
    // Get the average of the shakes
    position.y += mLastYShake;
    position.x += mLastXShake;
}

void PistonEngineCameraCommand::accept(IVisitor &visitor)
{
    visitor.visit(*this);
}
