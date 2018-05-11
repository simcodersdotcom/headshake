#include <stdio.h>

#include <CHeaders/XPLM/XPLMUtilities.h>

#include "settings/settingswriter.h"
#include "cameracontrol.h"
#include "cameracommands/gforcecameracommand.h"
#include "cameracommands/lookaheadcameracommand.h"
#include "cameracommands/pistonenginecameracommand.h"
#include "cameracommands/groundrollcameracommand.h"
#include "cameracommands/taxilookaheadcameracommand.h"
#include "cameracommands/guncameracommand.h"
#include "cameracommands/rotorcameracommand.h"
#include "cameracommands/touchdowncameracommand.h"
#include "helpers.h"

SettingsWriter::SettingsWriter()
{
    mFile = NULL;
}

SettingsWriter::~SettingsWriter()
{

}

void SettingsWriter::open()
{
    char path[1024];
    if (!get_settings_filepath(path)) {
        XPLMDebugString("HeadShake: Unable to get the prefs folder path.\n");
        return;
    }
    mFile = fopen(path, "w+");
    if (mFile == NULL) {
        XPLMDebugString("HeadShake: Unable to open (write) file ");
        XPLMDebugString(path);
    }
}

void SettingsWriter::close()
{
    if (mFile != NULL)
        fclose(mFile);
}

void SettingsWriter::visit(CameraControl &control)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracontrol.multimonitorcompat.enabled=%s\n", control.get_multimonitor_compatibility() ? "1" : "0");
        fprintf(mFile, "cameracontrol.enabled=%s\n", control.get_enabled() ? "1" : "0");
    }
}

void SettingsWriter::visit(GForceCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.gforce.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.gforce.pitch.response=%.0f\n", command.get_pitch_response());
        fprintf(mFile, "cameracommand.gforce.yaw.response=%.0f\n", command.get_yaw_response());
        fprintf(mFile, "cameracommand.gforce.acceleration.response=%.0f\n", command.get_acceleration_response());
    }
}

void SettingsWriter::visit(LookAheadCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.lookahead.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.lookahead.response=%.0f\n", command.get_response());
    }
}

void SettingsWriter::visit(PistonEngineCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.pistonengine.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.pistonengine.response=%.0f\n", command.get_response());
    }
}

void SettingsWriter::visit(GroundRollCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.groundroll.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.groundroll.response=%.0f\n", command.get_response());
    }
}

void SettingsWriter::visit(TaxiLookAheadCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.taxilookahead.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.taxilookahead.rudder.response=%.0f\n", command.get_rudder_response());
        fprintf(mFile, "cameracommand.taxilookahead.turn.response=%.0f\n", command.get_turn_response());
        fprintf(mFile, "cameracommand.taxilookahead.lean.response=%.0f\n", command.get_lean_response());
    }
}

void SettingsWriter::visit(TouchdownCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.touchdown.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.touchdown.response=%.0f\n", command.get_response());
    }
}

void SettingsWriter::visit(GunCameraCommand&)
{

}

void SettingsWriter::visit(RotorCameraCommand &command)
{
    if (mFile != NULL) {
        fprintf(mFile, "cameracommand.rotor.enabled=%s\n", command.is_enabled() ? "1" : "0");
        fprintf(mFile, "cameracommand.rotor.response=%.0f\n", command.get_response());
    }
}
