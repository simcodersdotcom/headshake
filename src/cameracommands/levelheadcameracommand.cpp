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

void LevelHeadCameraCommand::execute(CameraPosition &position)
{
	float currentBank;

	position.roll += mLastRoll;

	mLastRoll = 0;

	if (!pEnabled) return;

	currentBank = XPLMGetDataf(mRollRef);

	if (currentBank < 0) mLastRoll = (std::max(-mMaxBankAngle, currentBank)) * (mResponse / 100.0f);
	else mLastRoll = (std::min(mMaxBankAngle, currentBank)) * (mResponse / 100.0f);

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
