#ifndef CAMERACOMMAND_H
#define CAMERACOMMAND_H

#include <CHeaders/XPLM/XPLMDefs.h>

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
        virtual void execute(CameraPosition&, float);
        virtual void accept(IVisitor&) = 0;
        virtual void on_view_changed(int);
        virtual void on_enable();
        virtual void on_disable();
        virtual void on_receiving_message(XPLMPluginID, int, void*);
        void reset_blend();
    protected:
        float get_blend_ratio() const;
        bool pEnabled;
    private:
        float mBlendTime;
};

#endif // CAMERACOMMAND_H
