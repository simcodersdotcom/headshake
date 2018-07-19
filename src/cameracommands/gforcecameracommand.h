#ifndef GFORCECAMERACOMMAND_H
#define GFORCECAMERACOMMAND_H

#include <vector>

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

class GForceCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        GForceCameraCommand();
        /** Default destructor */
        virtual ~GForceCameraCommand();
        void execute(CameraPosition&);
        void accept(IVisitor&);
        /** Implementation methods */
        void set_pitch_response(float);
        float get_pitch_response();
        void set_acceleration_response(float);
        float get_acceleration_response();
        void set_yaw_response(float response);
        float get_yaw_response();
    protected:
    private:
        unsigned int mDamper;
        std::vector<float> mPitchFilter;
        std::vector<float> mYawFilter;
        std::vector<float> mZFilter;
        std::vector<float> mAlphaFilter;
        float mLastPitch;
        float mLastYaw;
        float mLastRoll;
        float mLastZ;
        float mLastX;
        float mPitchResponse;
        float mYawResponse;
        float mZResponse;
        float mLastNormG;
        float mLastAxialG;
        XPLMDataRef mGNormalDataRef;
        XPLMDataRef mGAxialDataRef;
        XPLMDataRef mGSideDataRef;
        XPLMDataRef mRadioAltDataRef;
        XPLMDataRef mOnGroundDataRef;
        XPLMDataRef mBrakesDataRef;
};

#endif // GFORCECAMERACOMMAND_H
