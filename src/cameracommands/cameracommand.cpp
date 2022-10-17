#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

CameraCommand::CameraCommand()
{
    pEnabled = true;
    mBlendTime = 1;
}

CameraCommand::~CameraCommand()
{

}

bool CameraCommand::is_enabled()
{
    return pEnabled;
}


void CameraCommand::set_enabled(bool enabled)
{
    pEnabled = enabled;
}

void CameraCommand::reset_blend()
{
    mBlendTime = 0;
}

void CameraCommand::toggle()
{
	pEnabled = !pEnabled;
}

void CameraCommand::execute(CameraPosition&, float elapsedTime)
{
    if (mBlendTime < 2) {
        mBlendTime += elapsedTime;
    }
}

// Executed when the view type changes
void CameraCommand::on_view_changed(int)
{

}

void CameraCommand::on_enable()
{

}

void CameraCommand::on_disable()
{

}

void CameraCommand::on_receiving_message(XPLMPluginID, int, void*)
{

}

float CameraCommand::get_blend_ratio() const
{
    return mBlendTime / 2;
}
