#ifndef GUNCAMERACOMMAND_H
#define GUNCAMERACOMMAND_H

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"

class GunCameraCommand : public CameraCommand
{
    public:
        /** Default constructor */
        GunCameraCommand();
        /** Default destructor */
        virtual ~GunCameraCommand();
        void execute(CameraPosition&);
        void accept(IVisitor&);
    protected:
    private:
};

#endif // GUNCAMERACOMMAND_H
