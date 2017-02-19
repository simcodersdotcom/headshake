#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <vector>

#include <CHeaders/XPLM/XPLMDataAccess.h>
#include <CHeaders/XPLM/XPLMUtilities.h>

#include "cameracommands/cameracommand.h"
#include "cameraposition.h"
#include "interfaces/ivisitable.h"

struct CameraPosition;
class CameraControl : IVisitable
{
public:
    static CameraControl* get_instance();
    float control();
    void accept(IVisitor&);
    void on_enable();
    void on_disable();
    bool get_multimonitor_compatibility();
    void set_multimonitor_compatibility(bool);
    bool error();
    CameraPosition get_offset();
    bool get_override();
    void set_override(bool);
protected:
private:
    /** Singleton */
    CameraControl();  // Private so that it can  not be called
    CameraControl(CameraControl const&){};             // copy constructor is private
    CameraControl& operator=(CameraControl const&){ return *mInstance; };  // assignment operator is private
    static CameraControl* mInstance;
    virtual ~CameraControl();
    /** Implementation */
    void freeze();
    std::vector<CameraCommand*> mCommands;
    unsigned int mCommandsSize;
    CameraPosition mLastPos;
    CameraPosition mInitialPos;
    CameraPosition mCameraOffset;
    int mLastCameraType;
    bool mPositionInited;
    bool mFreezed1;
    bool mFreezed2;
    bool mError;
    bool mMultimonitorCompatibility;
    bool mOverride;
    float mLastJoyPitch;
    float mLastJoyYaw;
    /** Published DataRefs **/
    XPLMDataRef mOverrideDataRef;
    XPLMDataRef mXDataRef;
    XPLMDataRef mYDataRef;
    XPLMDataRef mZDataRef;
    XPLMDataRef mYawDataRef;
    XPLMDataRef mPitchDataRef;
    XPLMDataRef mRollDataRef;
    /** DataRefs */
    XPLMDataRef mPausedDataRef;
    XPLMDataRef mCinemaVeriteDataRef;
    XPLMDataRef mViewTypeDataRef;
    XPLMDataRef mHeadHeadingDataRef;
    XPLMDataRef mHeadPitchDataRef;
    XPLMDataRef mHeadRollDataRef;
    XPLMDataRef mHeadXDataRef;
    XPLMDataRef mHeadYDataRef;
    XPLMDataRef mHeadZDataRef;
    XPLMDataRef mJoyAxisValues;
    XPLMDataRef mJoyAxisAssignments;
    /** Commands */
    std::vector<XPLMCommandRef> mStopCommands;
    std::vector<XPLMDataRef> mDrefs;
    unsigned int mStopCommandsSize;
};

#endif // CAMERACONTROL_H
