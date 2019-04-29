#if !defined(XPLM210)
#define XPLM210
#endif

#include <stddef.h>  // defines NULL
#include <CHeaders/XPLM/XPLMMenus.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMDisplay.h>
#include <CHeaders/XPLM/XPLMProcessing.h>
#include <CHeaders/Widgets/XPWidgets.h>
#include <CHeaders/Widgets/XPStandardWidgets.h>
#include <CHeaders/Widgets/XPWidgetDefs.h>
#include <CHeaders/Widgets/XPWidgetUtils.h>

#include "menu.h"
#include "cameracommands/gforcecameracommand.h"
#include "cameracommands/lookaheadcameracommand.h"
#include "cameracommands/pistonenginecameracommand.h"
#include "cameracommands/rotorcameracommand.h"
#include "cameracommands/groundrollcameracommand.h"
#include "cameracommands/taxilookaheadcameracommand.h"
#include "cameracommands/touchdowncameracommand.h"

// Custom messages
#define UPDATE_GFORCE_PITCH        1
#define UPDATE_GFORCE_YAW          2
#define UPDATE_GFORCE_ACCELERATION 3
#define UPDATE_GFORCE_LOOKAHEAD    4
#define UPDATE_GROUNDROLL          5
#define UPDATE_TAXI_LOOKAHEAD      6
#define UPDATE_PISTONENGINE        7
#define UPDATE_ROTOR               8
#define UPDATE_TOUCHDOWN           9

/**
|-------------------------------
|   Singleton and menu loader
|-------------------------------
*/

Menu* Menu::mInstance = NULL;
Menu* Menu::get_instance()
{
    if (!mInstance)
        mInstance = new Menu;
    return mInstance;
}

// Create the main menu entry SimCoders->HeadShake
void Menu::create_menu_entry(CameraControl &control)
{
    mCameraControl = &control;
    mId = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "SimCoders - HeadShake", 0, 1);
    XPLMMenuID subMenuId = XPLMCreateMenu("Settings", XPLMFindPluginsMenu(), mId, [](void*, void*) -> void {
        // When clicked, start visiting the control and the commands and draw the widgets
        Menu::mInstance->show();
    },  0);
    XPLMAppendMenuItem(subMenuId, "Settings", (void*)1, 1);

    mToggleCommand = XPLMCreateCommand("simcoders/headshake/toggle_menu", "Open/Close the settings menu");
    XPLMRegisterCommandHandler(mToggleCommand, Menu::toggle, true, 0);
}

void Menu::destroy_menu_entry()
{
    XPLMRemoveMenuItem(XPLMFindPluginsMenu(), mId);
    XPLMUnregisterCommandHandler(mToggleCommand, Menu::toggle, true, 0);
}

// Static: used to toggle the menu
int Menu::toggle(XPLMCommandRef, XPLMCommandPhase inPhase, void*)
{
    if (inPhase == xplm_CommandBegin) {
        if (Menu::mInstance->mWidgetId == nullptr) {
            Menu::mInstance->show();
        } else {
            XPDestroyWidget(Menu::mInstance->mWidgetId, 1);
            Menu::mInstance->mWidgetId = nullptr;
        }
    }
    return 1;
}

void Menu::show()
{
    mCameraControl->accept(*mInstance);
}

/**
|----------------------------------
|   Widget creation and handling
|----------------------------------
*/

Menu::Menu()
{
    // Define the main window width
    // The other values are calculated at run time
    mWidth = 700;
    mAdsHeight = 100;
    mWidgetId = nullptr;
}

Menu::~Menu()
{

}

// Create the main window and save the window id
void Menu::visit(CameraControl &control)
{
    int w, h, x1, x2;
    XPWidgetID subw;
    XPLMGetScreenSize(&w, &h);
    mHeight = (control.error() ? 560 : 510) + mAdsHeight;
    mLeft = (w - mWidth) / 2;
    mTop = (h + mHeight) / 2;
    mRight = mLeft + mWidth;
    int bottom = mTop - mHeight;
    XPWidgetID compatibilityButton;
    mShowAds = XPLMFindPluginBySignature("com.simcoders.rep") == XPLM_NO_PLUGIN_ID;

    // Create the Main Widget window
    mWidgetId = XPCreateWidget(mLeft, mTop, mRight, bottom, 1, "SimCoders - HeadShake", 1, 0, xpWidgetClass_MainWindow);
    XPSetWidgetProperty(mWidgetId, xpProperty_MainWindowHasCloseBoxes, 1);
    XPAddWidgetCallback(mWidgetId, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        // When the close buttons are clicked, close the window
        if (inMessage == xpMessage_CloseButtonPushed) {
            XPDestroyWidget(inWidget, 1);
            Menu::mInstance->mWidgetId = nullptr;
            return 1;
        }
        return 0;
    });
    // On error show a warning caption
    if (control.error()) {
        mTop -= 50;
        XPCreateWidget(mLeft + 10, mTop, mRight - 10, mTop - 1, 1, "WARNING: This plugin does not work when Cinema Verite is turned on.", 0, mWidgetId, xpWidgetClass_Caption);
    }

    // Set the REP reference
    int y = mTop - 23;
    x1 = mLeft + 20;
    x2 = mRight - 20;
    subw = XPCreateWidget(mLeft + 10, y, mRight - 10, y - mAdsHeight, 1, "Reality Expansion Pack Integration", 0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
    // Add the description
    XPCreateWidget(x1 - 5, y, x2, y - 30, 1, "REALITY EXPANSION PACK INTEGRATION", 0, mWidgetId, xpWidgetClass_Caption);
    if (mShowAds) {
        XPCreateWidget(x1 - 5, y, x2, y - 90, 1, "HeadShake can work in pair with the Reality Expansion Pack, our hyper-realistic addon for X-Plane.", 0, mWidgetId, xpWidgetClass_Caption);
        XPCreateWidget(x1 - 5, y, x2, y - 120, 1, "It adds a damages & maintenance system, realistic flight dynamics and much more to X-Plane.", 0, mWidgetId, xpWidgetClass_Caption);
        XPCreateWidget(x1 - 5, y, x2, y - 150, 1, "Get more informations at https://www.simcoders.com/rep", 0, mWidgetId, xpWidgetClass_Caption);
    } else {
        XPCreateWidget(x1 - 5, y, x2, y - 85, 1, "HeadShake is correctly working in pair with the Reality Expansion Pack.", 0, mWidgetId, xpWidgetClass_Caption);
        XPCreateWidget(x1 - 5, y, x2, y - 130, 1, "Thank you for being a REP licence holder. We are a small company and your help is very much appreciated.", 0, mWidgetId, xpWidgetClass_Caption);
        XPCreateWidget(x1 - 5, y, x2, y - 160, 1, "We wish you blue skies with REP and HeadShake!", 0, mWidgetId, xpWidgetClass_Caption);
    }

    // Move all the widgets down to make space for the ads
    mTop -= mAdsHeight;

    // Create the general "Enable/Disable" menu
    y = mTop - 30;
    x1 = mLeft + 10;
    x2 = x1 + mWidth / 2 - 20;
    subw = XPCreateWidget(x1, y, x2, y - 150, 1, "General", 0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
    x1 += 10;
    x2 -= 10;
    y -= 10;
    // Add the compatibility checkbox
    compatibilityButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable HeadShake", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonState, control.get_enabled());
    XPAddWidgetCallback(compatibilityButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mCameraControl->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });
    y -= 10;
    // Add the description
    XPCreateWidget(x1, y, x1 + 10, y - 30, 1, "This is the general toggle for HeadShake.", 0, mWidgetId, xpWidgetClass_Caption);
    XPCreateWidget(x1, y, x1 + 10, y - 60, 1, "Enable it if you want to use this plugin.", 0, mWidgetId, xpWidgetClass_Caption);
    XPCreateWidget(x1, y, x1 + 10, y - 120, 1, "The following commands are also available:", 0, mWidgetId, xpWidgetClass_Caption);
    XPCreateWidget(x1, y, x1 + 10, y - 150, 1, " - simcoders/headshake/toggle_headshake", 0, mWidgetId, xpWidgetClass_Caption);
    XPCreateWidget(x1, y, x1 + 10, y - 180, 1, " - simcoders/headshake/toggle_menu", 0, mWidgetId, xpWidgetClass_Caption);

    // Create the multimonitor compatibility menu
    // Place it at the bottom
    // Set the subwindow top like main top minus the gforce height,
    // the engine vibrations height, the ground roll height, the touchdown height, the piston engine height, the roto height
    // and some padding
    y = mTop - 180 - 10 - 70 - 10 - 70 - 10 - 70 - 10;
    x1 = mLeft + mWidth / 2 + 10;
    x2 = x1 + mWidth / 2 - 20;
    subw = XPCreateWidget(x1, y, x2, y - 70, 1, "Compatibility", 0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
    x1 += 10;
    x2 -= 10;
    y -= 10;
    // Add the compatibility checkbox
    compatibilityButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable multimonitor compatibility", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(compatibilityButton, xpProperty_ButtonState, control.get_multimonitor_compatibility());
    XPAddWidgetCallback(compatibilityButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mCameraControl->set_multimonitor_compatibility(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });
    y -= 10;
    // Add the description
    XPCreateWidget(x1, y, x1 + 10, y - 30, 1, "Enable this if you use more screens.", 0, mWidgetId, xpWidgetClass_Caption);
}

// Create and handle the gforce subwindow
void Menu::visit(GForceCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2, y;
    XPWidgetID enableButton;
    XPWidgetID pitchScrollbar;
    XPWidgetID pitchLabel;
    XPWidgetID accelerationScrollbar;
    XPWidgetID accelerationLabel;
    XPWidgetID yawScrollbar;
    XPWidgetID yawLabel;
    char buffer[64];

    mGforceCameraCommand = &command;

    x1 = mLeft + mWidth / 2 + 10;
    x2 = x1 + mWidth / 2 - 20;

    subw = XPCreateWidget(x1, mTop - 30, x2, mTop - 180, 1, "GForce Effect Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 += 10;
    x2 -= 10;

    // Add the enable checkbox
    y = mTop - 40;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the g-force effects", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mGforceCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the pitch sensitivity label + scrollbar
    y = y - 20;
    sprintf(buffer, "Pitch response: %.0f", mGforceCameraCommand->get_pitch_response());
    pitchLabel = XPCreateWidget(x1 - 5, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(pitchLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_GFORCE_PITCH) {
            char mbuffer[32];
            sprintf(mbuffer, "Pitch response: %.0f", Menu::mInstance->mGforceCameraCommand->get_pitch_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    pitchScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(pitchScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(pitchScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(pitchScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_pitch_response());
    XPSetWidgetProperty(pitchScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(pitchScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mGforceCameraCommand->set_pitch_response((float)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            // Update the label
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_GFORCE_PITCH, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });

    // Add the yaw sensitivity label + scrollbar
    y = y - 20;
    sprintf(buffer, "Yaw response: %.0f", mGforceCameraCommand->get_yaw_response());
    yawLabel = XPCreateWidget(x1 - 5, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(yawLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_GFORCE_YAW) {
            char mbuffer[32];
            sprintf(mbuffer, "Yaw response: %.0f", Menu::mInstance->mGforceCameraCommand->get_yaw_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    yawScrollbar = XPCreateWidget(x1 + 5 , y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(yawScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(yawScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(yawScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_yaw_response());
    XPSetWidgetProperty(yawScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(yawScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mGforceCameraCommand->set_yaw_response((float)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            // Update the label
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_GFORCE_YAW, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });

    // Add the acceleration sensitivity label + scrollbar
    y = y - 20;
    sprintf(buffer, "Speed change response: %.0f", mGforceCameraCommand->get_acceleration_response());
    accelerationLabel = XPCreateWidget(x1 - 5, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(accelerationLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_GFORCE_ACCELERATION) {
            char mbuffer[32];
            sprintf(mbuffer, "Speed change response: %.0f", Menu::mInstance->mGforceCameraCommand->get_acceleration_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    accelerationScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(accelerationScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(accelerationScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(accelerationScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_acceleration_response());
    XPSetWidgetProperty(accelerationScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(accelerationScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mGforceCameraCommand->set_acceleration_response((float)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            // Update the label
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_GFORCE_ACCELERATION, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });


}

void Menu::visit(LookAheadCameraCommand &command)
{
    XPWidgetID subw;
    XPWidgetID lookaheadScrollbar;
    XPWidgetID lookaheadLabel;
    XPWidgetID lookaheadEnabledButton;
    char buffer[64];
    int x1, x2, y = mTop - 180 - 10;

    mLookAheadCameraCommand = &command;
    subw = XPCreateWidget(mLeft + 10, y, mRight - ( mWidth / 2 ) - 10, y - 70, 1, "LookAhead Effect Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 = mLeft + 20;
    x2 = mRight - 20;

    // Add the enable checkbox
    y = y - 10;
    lookaheadEnabledButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the look ahead effects", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(lookaheadEnabledButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(lookaheadEnabledButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(lookaheadEnabledButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(lookaheadEnabledButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mLookAheadCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the sensitivity label + scrollbar
    y = y - 20;
    sprintf(buffer, "Lookahead maximum angle: %.0f degrees", mLookAheadCameraCommand->get_response());
    lookaheadLabel = XPCreateWidget(x1 - 5, y, x2 - ( mWidth / 2 ), y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(lookaheadLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_GFORCE_LOOKAHEAD) {
            char mbuffer[64];
            sprintf(mbuffer, "Lookahead maximum angle: %.0f degrees", Menu::mInstance->mLookAheadCameraCommand->get_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    lookaheadScrollbar = XPCreateWidget(x1 + 5, y, x2 - ( mWidth / 2 ) - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(lookaheadScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(lookaheadScrollbar, xpProperty_ScrollBarMax, 90);
    XPSetWidgetProperty(lookaheadScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_response());
    XPSetWidgetProperty(lookaheadScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(lookaheadScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mLookAheadCameraCommand->set_response((float)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            // Update the label
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_GFORCE_LOOKAHEAD, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

void Menu::visit(GroundRollCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2;
    // Set the subwindow top like main top minus the gforce height
    int y = mTop - 180 - 10 - 70 - 10;
    XPWidgetID enableButton;
    XPWidgetID responseScrollbar;
    XPWidgetID responseLabel;
    char buffer[16];

    mGroundRollCameraCommand = &command;
    subw = XPCreateWidget( mLeft + 10, y, mRight - ( mWidth / 2 ) - 10, y - 70, 1, "Ground Roll Shaking Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 = mLeft + 20;
    x2 = mRight - 20;

    // Add the enable checkbox
    y = y - 10;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the ground roll vibrations", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mGroundRollCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Response: %.0f", mGroundRollCameraCommand->get_response());
    responseLabel = XPCreateWidget(x1, y, x2 - ( mWidth / 2 ), y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_GROUNDROLL) {
            char mbuffer[16];
            sprintf(mbuffer, "Response: %.0f", Menu::mInstance->mGroundRollCameraCommand->get_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - ( mWidth / 2 ) - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mGroundRollCameraCommand->set_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_GROUNDROLL, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

void Menu::visit(TaxiLookAheadCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2;
    // Set the subwindow top like main top minus the gforce height
    int y = mTop - 180 - 10;
    XPWidgetID enableButton;
    XPWidgetID responseScrollbar;
    XPWidgetID responseLabel;
    char buffer[32];

    mTaxiLookAheadCameraCommand = &command;
    subw = XPCreateWidget(mLeft + ( mWidth / 2 ) + 10, y, mRight - 10, y - 70 - 10 - 70, 1, "Taxi LookAhead Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 = mLeft + ( mWidth / 2 ) + 20;
    x2 = mRight - 20;

    // Add the enable checkbox
    y = y - 10;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the taxi lookahead", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mTaxiLookAheadCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the rudder response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Rudder response: %.0f", mTaxiLookAheadCameraCommand->get_rudder_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD) {
            char mbuffer[32];
            sprintf(mbuffer, "Rudder response: %.0f", Menu::mInstance->mTaxiLookAheadCameraCommand->get_rudder_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_rudder_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mTaxiLookAheadCameraCommand->set_rudder_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });

    // Add the turn response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Turn response: %.0f", mTaxiLookAheadCameraCommand->get_turn_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD) {
            char mbuffer[32];
            sprintf(mbuffer, "Turn response: %.0f", Menu::mInstance->mTaxiLookAheadCameraCommand->get_turn_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_turn_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mTaxiLookAheadCameraCommand->set_turn_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });

    // Add the lean response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Lean Response: %.0f", mTaxiLookAheadCameraCommand->get_lean_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD) {
            char mbuffer[32];
            sprintf(mbuffer, "Lean Response: %.0f", Menu::mInstance->mTaxiLookAheadCameraCommand->get_lean_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_lean_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mTaxiLookAheadCameraCommand->set_lean_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_TAXI_LOOKAHEAD, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

void Menu::visit(TouchdownCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2;
    // Set the subwindow top like main top minus the gforce height minus the ground roll height
    int y = mTop - 180 - 10 - 70 - 10 - 70 - 10;
    XPWidgetID enableButton;
    XPWidgetID responseScrollbar;
    XPWidgetID responseLabel;
    char buffer[16];

    mTouchdownCameraCommand = &command;
    subw = XPCreateWidget(mLeft + 10, y, mLeft + mWidth / 2 - 10, y - 70, 1, "Touchdown effect Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 = mLeft + 20;
    x2 = mLeft + mWidth / 2 - 20;

    // Add the enable checkbox
    y = y - 10;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the touchdown effect", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mTouchdownCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Response: %.0f", mTouchdownCameraCommand->get_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_TOUCHDOWN) {
            char mbuffer[16];
            sprintf(mbuffer, "Response: %.0f", Menu::mInstance->mTouchdownCameraCommand->get_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mTouchdownCameraCommand->set_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_TOUCHDOWN, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

// Create and handle the piston engine subwindow
void Menu::visit(PistonEngineCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2;
    // Set the subwindow top like main top minus the gforce height minus some padding and the ground roll height minus the touchdown height
    int y = mTop - 180 - 10 - 70 - 10 - 70 - 10;
    XPWidgetID enableButton;
    XPWidgetID responseScrollbar;
    XPWidgetID responseLabel;
    char buffer[16];

    mPistonEngineCameraCommand = &command;

    x1 = mLeft + mWidth / 2 + 10;
    x2 = x1 + mWidth / 2 - 20;

    subw = XPCreateWidget(x1, y, x2, y - 70, 1, "Piston Engine Vibrations Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 += 10;
    x2 -= 10;

    // Add the enable checkbox
    y = y - 10;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the piston engine vibrations", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mPistonEngineCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Response: %.0f", mPistonEngineCameraCommand->get_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_PISTONENGINE) {
            char mbuffer[16];
            sprintf(mbuffer, "Response: %.0f", Menu::mInstance->mPistonEngineCameraCommand->get_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mPistonEngineCameraCommand->set_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_PISTONENGINE, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

void Menu::visit(RotorCameraCommand &command)
{
    XPWidgetID subw;
    int x1, x2;
    // Set the subwindow top like main top minus the gforce height,
    // the engine vibrations height, the ground roll height, the touchdown height, the piston engine height and some padding
    int y = mTop - 180 - 10 - 70 - 10 - 70 - 10 - 70 - 10;
    XPWidgetID enableButton;
    XPWidgetID responseScrollbar;
    XPWidgetID responseLabel;
    char buffer[16];

    mRotorCameraCommand = &command;
    subw = XPCreateWidget(mLeft + 10, y, mLeft + mWidth / 2 - 10, y - 70, 1, "Rotor Vibrations Settings",  0, mWidgetId, xpWidgetClass_SubWindow);
    XPSetWidgetProperty(subw, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

    x1 = mLeft + 20;
    x2 = mLeft + mWidth / 2 - 20;

    // Add the enable checkbox
    y = y - 10;
    enableButton = XPCreateWidget(x1, y, x1 + 10, y - 10, 1, " Enable the rotor vibrations", 0, mWidgetId, xpWidgetClass_Button);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(enableButton, xpProperty_ButtonState, command.is_enabled());
    XPAddWidgetCallback(enableButton, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ButtonStateChanged) {
            Menu::mInstance->mRotorCameraCommand->set_enabled(XPGetWidgetProperty(inWidget, xpProperty_ButtonState, &inExit));
            return 1;
        }
        return 0;
    });

    // Add the response label + scrollbar
    y = y - 20;
    sprintf(buffer, "Response: %.0f", mRotorCameraCommand->get_response());
    responseLabel = XPCreateWidget(x1, y, x2, y - 10, 1, buffer, 0, mWidgetId, xpWidgetClass_Caption);
    // On message received update the label
    XPAddWidgetCallback(responseLabel, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        if (inMessage == xpMsg_UserStart + UPDATE_ROTOR) {
            char mbuffer[16];
            sprintf(mbuffer, "Response: %.0f", Menu::mInstance->mRotorCameraCommand->get_response());
            XPSetWidgetDescriptor(inWidget, mbuffer);
            return 1;
        }
        return 0;
    });
    y = y - 20;
    responseScrollbar = XPCreateWidget(x1 + 5, y, x2 - 5, y - 10, 1, "", 0, mWidgetId, xpWidgetClass_ScrollBar);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMin, 1);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarMax, 100);
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarSliderPosition, (int)command.get_response());
    XPSetWidgetProperty(responseScrollbar, xpProperty_ScrollBarType, xpScrollBarTypeSlider);
    XPAddWidgetCallback(responseScrollbar, [](XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t, intptr_t) -> int {
        int inExit;
        if (inMessage == xpMsg_ScrollBarSliderPositionChanged) {
            Menu::mInstance->mRotorCameraCommand->set_response((int)(XPGetWidgetProperty(inWidget, xpProperty_ScrollBarSliderPosition, &inExit)));
            XPSendMessageToWidget(Menu::mInstance->mWidgetId, xpMsg_UserStart + UPDATE_ROTOR, xpMode_Recursive, 0, 0);
            return 1;
        }
        return 0;
    });
}

void Menu::visit(GunCameraCommand&)
{

}
