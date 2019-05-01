#ifndef IVISITOR_H
#define IVISITOR_H

class CameraControl;
class GForceCameraCommand;
class LookAheadCameraCommand;
class PistonEngineCameraCommand;
class RotorCameraCommand;
class GroundRollCameraCommand;
class TaxiLookAheadCameraCommand;
class TouchdownCameraCommand;
class LevelHeadCameraCommand;

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
        virtual void visit(GroundRollCameraCommand&) = 0;
        virtual void visit(TaxiLookAheadCameraCommand&) = 0;
        virtual void visit(TouchdownCameraCommand&) = 0;
		virtual void visit(LevelHeadCameraCommand&) = 0;
};

#endif // IVISITOR_H
