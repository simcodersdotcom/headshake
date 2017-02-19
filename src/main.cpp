/*
 * main.cpp
 *
 */
#include <stdio.h>
#include <string.h>

#include <CHeaders/XPLM/XPLMDisplay.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/XPLM/XPLMPlugin.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "cameracontrol.h"
#include "menu.h"
#include "settings/settingswriter.h"
#include "settings/settingsreader.h"

// Define the callbacks
float FlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void *refcon);

PLUGIN_API int XPluginStart(
    char *		outName,
    char *		outSig,
    char *		outDesc)
{
    // Define the plugin properties
    strcpy(outName, "SimCoders - HeadShake 1.5");
    strcpy(outSig, "com.simcoders.headshake");
    strcpy(outDesc, "A plugin that moves the view in the virtual cockpit basing on the g force values.");
    // Setup the features
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
}

PLUGIN_API int XPluginEnable(void)
{
    SettingsReader *reader;

    // Enable the camera
    CameraControl::get_instance()->on_enable();
    // Read the settings
    reader = new SettingsReader;
    CameraControl::get_instance()->accept(*reader);
    delete reader;
    // Create the menu entry
    Menu::get_instance()->create_menu_entry(*(CameraControl::get_instance()));
    // Register the callbacks
    XPLMRegisterFlightLoopCallback(FlightLoopCallback, -1, 0);
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
    SettingsWriter *writer;

    // Unregister the callbacks
    XPLMUnregisterFlightLoopCallback(FlightLoopCallback, 0);
    CameraControl::get_instance()->on_disable();
    // Save the settings
    writer = new SettingsWriter;
    writer->open();
    CameraControl::get_instance()->accept(*writer);
    writer->close();
    delete writer;

    Menu::get_instance()->destroy_menu_entry();
}

PLUGIN_API void XPluginReceiveMessage(
    XPLMPluginID,
    long,
    void*)
{

}

float FlightLoopCallback(
    float, // Time since the last call
    float, // Time since the last loop
    int, // Loop counter
    void*)
{
    return CameraControl::get_instance()->control();
}
