#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

CameraCommand::CameraCommand()
{
    pEnabled = true;
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

void CameraCommand::toggle()
{
	pEnabled = !pEnabled;
}

void CameraCommand::execute(CameraPosition&)
{

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
