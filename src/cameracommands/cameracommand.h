#ifndef CAMERACOMMAND_H
#define CAMERACOMMAND_H

#include "interfaces/ivisitable.h"
#include "cameraposition.h"

class CameraCommand : public IVisitable
{
    public:
        /** Default constructor */
        CameraCommand();
        /** Default destructor */
        virtual ~CameraCommand();
        virtual bool is_enabled();
        virtual void set_enabled(bool);
		virtual void toggle();
        virtual void execute(CameraPosition&);
        virtual void accept(IVisitor&) = 0;
        virtual void on_view_changed(int);
        virtual void on_enable();
        virtual void on_disable();
    protected:
        bool pEnabled;
    private:
};

#endif // CAMERACOMMAND_H
