#ifndef MENU_H
#define MENU_H

#include <CHeaders/Widgets/XPWidgets.h>
#include <CHeaders/XPLM/XPLMMenus.h>
#include <CHeaders/XPLM/XPLMUtilities.h>
#include <CHeaders/XPLM/XPLMPlugin.h>

#include "interfaces/ivisitor.h"
#include "interfaces/ivisitable.h"
#include "cameracontrol.h"

class Menu : public IVisitor
{
    public:
        static Menu* get_instance();
        void create_menu_entry(CameraControl &);
        void destroy_menu_entry();
        // Visitors
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
        /**
        Singleton
        */
        Menu();  // Private so that it can  not be called
        Menu(Menu const&){};             // copy constructor is private
        Menu& operator=(Menu const&){ return *mInstance; };  // assignment operator is private
        static Menu* mInstance;
        virtual ~Menu();

        /**
        Vars
        */
        XPWidgetID mWidgetId;
        int mId;
        int mWidth;
        int mHeight;
        int mLeft;
        int mTop;
        int mRight;
        int mAdsHeight;
        bool mShowAds;
        CameraControl* mCameraControl;
        GForceCameraCommand* mGforceCameraCommand;
        LookAheadCameraCommand* mLookAheadCameraCommand;
        PistonEngineCameraCommand* mPistonEngineCameraCommand;
        RotorCameraCommand* mRotorCameraCommand;
        GroundRollCameraCommand* mGroundRollCameraCommand;
        TouchdownCameraCommand* mTouchdownCameraCommand;
};

#endif // MENU_H
