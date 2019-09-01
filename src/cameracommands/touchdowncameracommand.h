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
        virtual float get_last_roll();
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
        XPLMDataRef mVerticalSpeedDataRef;
        XPLMDataRef mOnGroundDataRef;
        XPLMDataRef mRadioAltDataRef;
};

#endif // TOUCHDOWNCAMERACOMMAND_H
