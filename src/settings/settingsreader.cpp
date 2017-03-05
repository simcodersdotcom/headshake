#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "settings/settingsreader.h"
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

SettingsReader::SettingsReader()
{
    char path[1024];
    std::ifstream file;
    std::string key, value, temp;
    std::size_t position;

    if (!get_settings_filepath(path))
        return;
    file.open(path);
    if (!file.is_open())
        return;
    while (std::getline(file, temp)) {
        position = temp.find("=");
        if (position != std::string::npos)
            mMap.insert(std::pair<std::string, std::string>(temp.substr(0, position), temp.substr(position + 1)));
    }
    file.close();
}

SettingsReader::~SettingsReader()
{

}

void SettingsReader::visit(CameraControl &control)
{
    if (mMap.find("cameracontrol.multimonitorcompat.enabled") != mMap.end())
        control.set_multimonitor_compatibility(mMap["cameracontrol.multimonitorcompat.enabled"].compare("1") == 0);
}

void SettingsReader::visit(GForceCameraCommand &command)
{
    if (mMap.find("cameracommand.gforce.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.gforce.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.gforce.pitch.response") != mMap.end())
        command.set_pitch_response(atof(mMap["cameracommand.gforce.pitch.response"].c_str()));
    if (mMap.find("cameracommand.gforce.yaw.response") != mMap.end())
        command.set_yaw_response(atof(mMap["cameracommand.gforce.yaw.response"].c_str()));
    if (mMap.find("cameracommand.gforce.acceleration.response") != mMap.end())
        command.set_acceleration_response(atof(mMap["cameracommand.gforce.acceleration.response"].c_str()));
}

void SettingsReader::visit(LookAheadCameraCommand &command)
{
    if (mMap.find("cameracommand.lookahead.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.lookahead.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.lookahead.response") != mMap.end())
        command.set_response(atof(mMap["cameracommand.lookahead.response"].c_str()));
}

void SettingsReader::visit(PistonEngineCameraCommand &command)
{
    if (mMap.find("cameracommand.pistonengine.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.pistonengine.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.pistonengine.response") != mMap.end())
        command.set_response(atof(mMap["cameracommand.pistonengine.response"].c_str()));
}

void SettingsReader::visit(GroundRollCameraCommand &command)
{
    if (mMap.find("cameracommand.groundroll.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.groundroll.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.groundroll.response") != mMap.end())
        command.set_response(atof(mMap["cameracommand.groundroll.response"].c_str()));
}

void SettingsReader::visit(TaxiLookAheadCameraCommand &command)
{
    if (mMap.find("cameracommand.taxilookahead.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.taxilookahead.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.taxilookahead.rudder.response") != mMap.end())
        command.set_rudder_response(atof(mMap["cameracommand.taxilookahead.rudder.response"].c_str()));
    if (mMap.find("cameracommand.taxilookahead.turn.response") != mMap.end())
        command.set_turn_response(atof(mMap["cameracommand.taxilookahead.turn.response"].c_str()));
    if (mMap.find("cameracommand.taxilookahead.lean.response") != mMap.end())
        command.set_lean_response(atof(mMap["cameracommand.taxilookahead.lean.response"].c_str()));
}

void SettingsReader::visit(TouchdownCameraCommand &command)
{
    if (mMap.find("cameracommand.touchdown.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.touchdown.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.touchdown.response") != mMap.end())
        command.set_response(atof(mMap["cameracommand.touchdown.response"].c_str()));
}

void SettingsReader::visit(GunCameraCommand&)
{

}

void SettingsReader::visit(RotorCameraCommand &command)
{
    if (mMap.find("cameracommand.rotor.enabled") != mMap.end())
        command.set_enabled(mMap["cameracommand.rotor.enabled"].compare("1") == 0);
    if (mMap.find("cameracommand.rotor.response") != mMap.end())
        command.set_response(atof(mMap["cameracommand.rotor.response"].c_str()));
}
