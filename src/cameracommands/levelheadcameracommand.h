#ifndef LEVELHEADCAMERACOMMAND_H
#define LEVELHEADCAMERACOMMAND_H

#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "cameracommands/cameracommand.h"

class LevelHeadCameraCommand : public CameraCommand
{
public:
	/** Default constructor */
	LevelHeadCameraCommand();
	/** Default destructor */
	virtual ~LevelHeadCameraCommand();
	void execute(CameraPosition&, float) override;
	void accept(IVisitor&);
	void set_response(float response);
	float get_response();
	void set_max_bank(float maxBank);
	float get_max_bank();
        virtual void on_view_changed(int);
        virtual float get_last_roll();
        virtual void reset_last_roll();
protected:
private:
	float mLastRoll;
	float mResponse;
	float mMaxBankAngle;
	XPLMDataRef mRollRef;
};

#endif // LEVELHEADCAMERACOMMAND_H
