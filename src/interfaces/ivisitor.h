#ifndef IVISITOR_H
#define IVISITOR_H

class CameraControl;
class GForceCameraCommand;
class LookAheadCameraCommand;
class PistonEngineCameraCommand;
class RotorCameraCommand;
class GunCameraCommand;
class GroundRollCameraCommand;
class TaxiLookAheadCameraCommand;
class TouchdownCameraCommand;
class IVisitor
{
    public:
        /** Default constructor */
        IVisitor() {}
        /** Default destructor */
        virtual ~IVisitor() {}
        virtual void visit(CameraControl&) = 0;
        virtual void visit(GForceCameraCommand&) = 0;
        virtual void visit(LookAheadCameraCommand&) = 0;
        virtual void visit(PistonEngineCameraCommand&) = 0;
        virtual void visit(RotorCameraCommand&) = 0;
        virtual void visit(GunCameraCommand&) = 0;
        virtual void visit(GroundRollCameraCommand&) = 0;
        virtual void visit(TaxiLookAheadCameraCommand&) = 0;
        virtual void visit(TouchdownCameraCommand&) = 0;
};

#endif // IVISITOR_H
