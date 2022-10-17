#ifndef TOUCHDOWNCAMERACOMMAND_H
#define TOUCHDOWNCAMERACOMMAND_H

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"

class TouchdownCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        TouchdownCameraCommand();
        /** Default destructor */
        virtual ~TouchdownCameraCommand();
        void execute(CameraPosition&, float) override;
        void accept(IVisitor&);
        /** Implementation methods */
        void set_response(float);
        float get_response();

        // Nose bump
        XPLMDataRef mBumpInitialAmplitude;
        XPLMDataRef mBumpDecayRate;
        XPLMDataRef mBumpFrequency;
        XPLMDataRef mBumpRollInitialAmplitude;
        XPLMDataRef mBumpRollDecayRate;
        XPLMDataRef mBumpRollFrequency;
        void on_enable() override;
        void on_disable() override;
        void on_receiving_message(XPLMPluginID, int, void*) override;

    protected:
    private:
        bool mPrevOnGround;
        bool mTouchdown;
        float mResponse;
        float mTouchdownTime;
        float mVspeed;
        float mLastPitch;
        float mLastRoll;
        float mLastX;
        float mLastY;
        XPLMDataRef mVerticalSpeedDataRef;
        XPLMDataRef mOnGroundDataRef;
        XPLMDataRef mRadioAltDataRef;

        // Nose bump
        float bumpInitialAmplitude;
        float bumpDecayRate;
        float bumpFrequency;
        float bumpRollInitialAmplitude;
        float bumpRollDecayRate;
        float bumpRollFrequency;
        float noseWheelTouchdownTime;
        bool mNoseWheelPrevOnGround;
        int wheelsOnGround[10];
        XPLMDataRef mGearsOnGroundDataRef;
        XPLMDataRef mWheelZPositionsDataRef;
        XPLMDataRef mTrueThetaDataRef;
        short locateNoseWheelPosition(float (&wheelZPositions)[10]);
        short noseWheelPosition;
        float lastSavedPitchAngle;
        float lastSavedPitchAngleTime;
        short pitchAngleCounter;
};

#endif // TOUCHDOWNCAMERACOMMAND_H
