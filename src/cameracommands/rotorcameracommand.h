#ifndef ROTORCAMERACOMMAND_H
#define ROTORCAMERACOMMAND_H

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

class RotorCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        RotorCameraCommand();
        /** Default destructor */
        virtual ~RotorCameraCommand();
        void execute(CameraPosition&, float) override;
        void accept(IVisitor&);
        void set_response(float response) { mResponse = response; }
        float get_response() { return mResponse; }
    protected:
    private:
        XPLMDataRef mRotorRpmDataRef;
        XPLMDataRef mRotorAngleDataRef;
        XPLMDataRef mPropTypeDataRef;
        XPLMDataRef mNumBladesDataRef;
        XPLMDataRef mPropMassDataRef;
        XPLMDataRef mOnGroundDataRef;
        float mLastYaw;
        float mResponse;
};

#endif // ROTORCAMERACOMMAND_H
