#ifndef SETTINGSWRITER_H
#define SETTINGSWRITER_H

#include <stdio.h>

#include "interfaces/ivisitor.h"

class SettingsWriter : public IVisitor
{
    public:
        /** Default constructor */
        SettingsWriter();
        /** Default destructor */
        virtual ~SettingsWriter();
        void open();
        void close();
        void visit(CameraControl&);
        void visit(GForceCameraCommand&);
        void visit(LookAheadCameraCommand&);
        void visit(PistonEngineCameraCommand&);
        void visit(RotorCameraCommand&);
        void visit(GunCameraCommand&);
        void visit(GroundRollCameraCommand&);
        void visit(TaxiLookAheadCameraCommand&);
        void visit(TouchdownCameraCommand&);
    protected:
    private:
        FILE *mFile;
};

#endif // SETTINGSWRITER_H
