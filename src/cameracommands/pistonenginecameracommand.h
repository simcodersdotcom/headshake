#ifndef PISTONENGINECAMERACOMMAND_H
#define PISTONENGINECAMERACOMMAND_H

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

class PistonEngineCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        PistonEngineCameraCommand();
        /** Default destructor */
        virtual ~PistonEngineCameraCommand();
        void execute(CameraPosition&);
        void accept(IVisitor&);
        void set_response(float response) { mResponse = response; }
        float get_response() { return mResponse; }
        void on_enable() override;
        void on_disable() override;
    protected:
    private:
        /** Searched datarefs */
        XPLMDataRef mNumEngineDataRef;
        XPLMDataRef mEngineTypeDataRef;
        XPLMDataRef mEngineRotationDataRef;
        XPLMDataRef mEngineRpmDataRef;
        XPLMDataRef mOnGroundDataRef;
        /** Published DataRefs **/
        XPLMDataRef mOverrideDataref;
        XPLMDataRef mVibrationRatioDataref;
        XPLMDataRef mVibrationFreqDataref;
        float mOverrideVibrationRatio[8];
        float mOverrideVibrationFreq[8];
        bool mOverride;
        float mLastXShake;
        float mLastYShake;
        float mResponse;
};

#endif // PISTONENGINECAMERACOMMAND_H
