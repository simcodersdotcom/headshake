#include <CHeaders/XPLM/XPLMDataAccess.h>

#include "helpers.h"
#include "cameraposition.h"
#include "interfaces/ivisitor.h"
#include "cameracommands/levelheadcameracommand.h"

LevelHeadCameraCommand::LevelHeadCameraCommand()
{
	mRollRef = XPLMFindDataRef("sim/flightmodel/position/phi");
	mLastRoll = 0;
	mResponse = 100;
	mMaxBankAngle = 20;
}

LevelHeadCameraCommand::~LevelHeadCameraCommand()
{
}

void LevelHeadCameraCommand::execute(CameraPosition &position, float elapsedTime)
{
	CameraCommand::execute(position, elapsedTime);

	float currentBank, targetRoll;

	position.roll += mLastRoll;

	if (!pEnabled) {
		mLastRoll = 0;
		return;
	}

	currentBank = XPLMGetDataf(mRollRef);

	if (currentBank < 0) {
                if (currentBank < -90.0f) {
                        targetRoll = -mMaxBankAngle * (1.0 - ((-currentBank - 90.0f) / 90.0f)) * (mResponse / 100.0f);
                }
                else {
                        targetRoll = (std::max(-mMaxBankAngle, currentBank)) * (mResponse / 100.0f);
                }
	} 
	else {
                if (currentBank > 90.0f) {
                        targetRoll = mMaxBankAngle * (1.0 - ((currentBank - 90.0f) / 90.0f)) * (mResponse / 100.0f);
                }
                else {
                        targetRoll = (std::min(mMaxBankAngle, currentBank)) * (mResponse / 100.0f);
                }
	}

	if (get_blend_ratio() < 1) {
		if (mLastRoll > targetRoll) {
			mLastRoll -= (mLastRoll - targetRoll) * get_blend_ratio();
		}
		else {
			mLastRoll += (targetRoll - mLastRoll) * get_blend_ratio();
		}
	}
	else {
		mLastRoll = targetRoll;
	}
	position.roll -= mLastRoll;
}

void LevelHeadCameraCommand::accept(IVisitor &visitor)
{
	visitor.visit(*this);
}

void LevelHeadCameraCommand::set_response(float response)
{
	mResponse = response;
}

float LevelHeadCameraCommand::get_response()
{
	return mResponse;
}

void LevelHeadCameraCommand::set_max_bank(float maxBank)
{
	mMaxBankAngle = maxBank;
}

float LevelHeadCameraCommand::get_max_bank()
{
	return mMaxBankAngle;
}

float LevelHeadCameraCommand::get_last_roll()
{
  return mLastRoll;
}
