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
    float control(float);
    void accept(IVisitor&);
    void on_enable();
    void on_disable();
    bool get_multimonitor_compatibility() const;
    void set_multimonitor_compatibility(bool);
    bool error();
    CameraPosition get_offset();
    bool get_override() const;
	int get_xp_version();
    void set_override(bool);
    void set_enabled(bool);
	void reset_view();
    bool get_enabled() const;
protected:
private:
    /** Singleton */
    CameraControl();  // Private so that it can  not be called
    CameraControl(CameraControl const&){};             // copy constructor is private
    CameraControl& operator=(CameraControl const&){ return *mInstance; };  // assignment operator is private

    static int freeze(XPLMCommandRef, XPLMCommandPhase, void*);
    static int toggle_hs(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_gforce(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_lookahead(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_pistonvib(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_rotorvib(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_groundroll(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_taxilook(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_touchdown(XPLMCommandRef, XPLMCommandPhase, void*);
	static int toggle_levelhead(XPLMCommandRef, XPLMCommandPhase, void*);

    static CameraControl* mInstance;
    virtual ~CameraControl();
    /** Implementation */
    void freeze();
	std::vector<CameraCommand*> mCommands;
    CameraPosition mLastPos;
    CameraPosition mInitialPos;
    CameraPosition mCameraOffset;
    int mLastCameraType;
	int mXpVersion;
    bool mPositionInited;
    bool mFreezed1;
    bool mFreezed2;
    bool mEnabled;
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
    XPLMDataRef mG_LoadedCameraDataRef;
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
	XPLMCommandRef mEnabledCommand;
    XPLMCommandRef mStopCommand;
	XPLMCommandRef mGforceToggleCommand;
	XPLMCommandRef mLookAheadToggleCommand;
	XPLMCommandRef mPistonVibToggleCommand;
	XPLMCommandRef mRotorVibToggleCommand;
	XPLMCommandRef mGroundRollToggleCommand;
	XPLMCommandRef mTaxiLookToggleCommand;
	XPLMCommandRef mTouchdownToggleCommand;
	XPLMCommandRef mLevelHeadToggleCommand;
    std::vector<XPLMCommandRef> mStopCommands;
    std::vector<XPLMDataRef> mDrefs;
    unsigned int mStopCommandsSize;
};

#endif // CAMERACONTROL_H
