#ifndef SETTINGSREADER_H
#define SETTINGSREADER_H

#include "interfaces/ivisitor.h"

#include <map>
#include <iostream>

class SettingsReader : public IVisitor
{
    public:
        /** Default constructor */
        SettingsReader();
        /** Default destructor */
        virtual ~SettingsReader();
        void visit(CameraControl&);
        void visit(GForceCameraCommand&);
        void visit(LookAheadCameraCommand&);
        void visit(PistonEngineCameraCommand&);
        void visit(RotorCameraCommand&);
        void visit(GunCameraCommand&);
        void visit(GroundRollCameraCommand&);
        void visit(TouchdownCameraCommand&);
    protected:
    private:
        std::map<std::string, std::string> mMap;
};

#endif // SETTINGSREADER_H
