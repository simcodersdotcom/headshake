#ifndef TAXILOOKAHEADCAMERACOMMAND_H
#define TAXILOOKAHEADCAMERACOMMAND_H

#include <vector>

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

class TaxiLookAheadCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        TaxiLookAheadCameraCommand();
        /** Default destructor */
        virtual ~TaxiLookAheadCameraCommand();
        void execute(CameraPosition&, float) override;
        void accept(IVisitor&);
        /** Implementation methods */
        void set_rudder_response(float);
        float get_rudder_response() const;
        void set_turn_response(float);
        float get_turn_response() const;
        void set_lean_response(float);
        float get_lean_response() const;
    protected:
    private:
        std::vector<float> mRudderFilter;
        std::vector<float> mTurnFilter;
        std::vector<float> mGroundSpeedFilter;
        float mLastZ;
        float mLastY;
        float mLastYaw;
        float mLastRoll;
        float mTurnResponse;
        float mRudderResponse;
        float mLeanResponse;
        XPLMDataRef mOnGroundDataRef;
        XPLMDataRef mTurnDataRef;
        XPLMDataRef mGroundSpeedDataRef;
        XPLMDataRef mRudderDataRef;
};

#endif // GROUNDROLLCAMERACOMMAND_H
