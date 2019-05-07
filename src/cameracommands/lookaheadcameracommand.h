#ifndef LOOKAHEADCAMERACOMMAND_H
#define LOOKAHEADCAMERACOMMAND_H
#include <vector>

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"

class LookAheadCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        LookAheadCameraCommand();
        void execute(CameraPosition&, float) override;
        void accept(IVisitor&);
        /** Default destructor */
        virtual ~LookAheadCameraCommand();
        void set_response(float response);
        float get_response();
        void on_view_changed(int);
    protected:
    private:
        std::vector<float> mPitchFilter;
        XPLMDataRef mGNormalDataRef;
        float mLastAlpha;
        float mTargetAlpha;
        float mLastPitch;
        float mResponse;
        float mDamper;
};

#endif // LOOKAHEADCAMERACOMMAND_H
