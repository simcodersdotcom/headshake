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
        virtual void execute(CameraPosition&, float);
        virtual void accept(IVisitor&) = 0;
        virtual void on_view_changed(int);
        virtual void on_enable();
        virtual void on_disable();
        void reset_blend();
        virtual float get_last_roll();
    protected:
        float get_blend_ratio() const;
        bool pEnabled;
    private:
        float mBlendTime;
};

#endif // CAMERACOMMAND_H
