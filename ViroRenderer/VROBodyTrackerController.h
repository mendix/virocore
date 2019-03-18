//
//  VROBodyTrackerController.h
//  ViroSample
//
//  Copyright © 2018 Viro Media. All rights reserved.
//

#ifndef VROBodyTrackerController_h
#define VROBodyTrackerController_h

#include "VROARDeclarativeNode.h"
#include "VROSceneController.h"
#include "VRODefines.h"
#include "VROBodyTracker.h"
#include "VRORenderer.h"

#if VRO_PLATFORM_IOS
#import "VROViewAR.h"
#import <UIKit/UIKit.h>
#endif

/*
 States representing the current tracking fidelity of this VROBodyTrackerController.
 */
enum VROBodyTrackedState {
    // Root body part effector required for body tracking is not found.
    NotAvailable,

    // Joints for determining scale is not found.
    NoScalableJointsAvailable,

    // Limited tracking where root and some body effectors are found, but not all.
    LimitedEffectors,

    // Successfully achieved full tracking for all root and body effectors.
    FullEffectors,
};

/*
 Joint that is tracked by this controller.
 */
class VROBodyJoint {
public:
    VROBodyJoint() : _type(VROBodyJointType::Top), _confidence(0) {}
    VROBodyJoint(VROBodyJointType type, double confidence) :
        _type(type),
        _confidence(confidence),
        _projectedTransform(VROMatrix4f::identity()),
        _hasValidProjectedTransform(false){}
    
    const VROVector3f &getScreenCoords() const {
        return _screenCoords;
    }
    void setScreenCoords(VROVector3f screenCoords) {
        _screenCoords = screenCoords;
    }
    
    const VROMatrix4f &getProjectedTransform() const {
        return _projectedTransform;
    }
    void setProjectedTransform(VROMatrix4f transform) {
        _projectedTransform = transform;
        _hasValidProjectedTransform = true;
    }
    void clearProjectedTransform() {
        _projectedTransform.toIdentity();
        _hasValidProjectedTransform = false;
    }
    bool hasValidProjectedTransform() const {
        return _hasValidProjectedTransform;
    }
    
    void setSpawnTimeMs(double ts) {
        _spawnTimeMs = ts;
    }
    double getSpawnTimeMs() const {
        return _spawnTimeMs;
    }
    
    VROBodyJointType getType() const {
        return _type;
    }
    
    double getConfidence() const {
        return _confidence;
    }
    
private:
    VROVector3f _screenCoords;
    VROMatrix4f _projectedTransform;
    bool _hasValidProjectedTransform;
    VROBodyJointType _type;
    double _confidence;
    double _spawnTimeMs;
};

/*
 Delegate set on a VROBodyTrackerController for notifying listeners about VROBodyTrackedState updates.
 */
class VROBodyTrackerControllerDelegate {
public:
    /*
     Represents the positional ML joint data in world space and its corresponding
     2D screen space position.
     */
    struct VROJointPos {
        VROVector3f worldPosition;
        float screenPosX;
        float screenPosY;
    };

    VROBodyTrackerControllerDelegate() {};
    virtual ~VROBodyTrackerControllerDelegate() {};

    /*
     Triggered when the calibration finishes
     */
    virtual void onCalibrationFinished() = 0;
    
    /*
     Triggered when the VROBodyTrackedState for the attached VROBodyTrackerController has changed.
     */
    virtual void onBodyTrackStateUpdate(VROBodyTrackedState state) = 0;

    /*
     Triggered when the controller has processed new joints after it has been calibrated.

     TODO: Remove unnecessary joint maps after narrowing down which joint data to use and expose.
     */
    virtual void onJointUpdate(const std::map<VROBodyJointType, VROJointPos> &mlJointsFiltered,
                               const std::map<VROBodyJointType, VROVector3f> &mlJointsDampened,
                               const std::map<VROBodyJointType, VROMatrix4f> &modelJoints) = 0;
};

/*
 VROBodyTrackerController coordinates the filtering, projecting and feeding of body tracking data
 from the VROBodyTracker into the currently bounded 3D model's VROIKRig for driving body motion.
 */
class VROBodyTrackerController : public VROBodyTrackerDelegate,
                                 public std::enable_shared_from_this<VROBodyTrackerController> {
public:
    VROBodyTrackerController(std::shared_ptr<VRORenderer> renderer,
                             std::shared_ptr<VRODriver> driver,
                             std::shared_ptr<VRONode> sceneRoot);
    ~VROBodyTrackerController();

    /*
     True if the given 3D model has been successfully bounded to this controller.
     */
    bool bindModel(std::shared_ptr<VRONode> modelRootNode);

    /*
     Notifies the controller to start aligning the underlying 3D model's root with
     the latest ML joint data.
     */
    void startCalibration();

    /*
     Sets a VROBodyTrackerControllerDelegate on this controller for
     onBodyTrackStateUpdate() notifications.
     */
    void setDelegate(std::shared_ptr<VROBodyTrackerControllerDelegate> delegate);

    // VROBodyTrackerDelegate
    void onBodyJointsFound(const VROPoseFrame &joints);

private:
                                     
    /*
     Set renderer needed for performing Ar Hit tests.
     */
    std::shared_ptr<VRORenderer> _renderer;

    /*
     The current VROBodyTrackedState of this controller.
     */
    VROBodyTrackedState _currentTrackedState;

    /*
     Set VROBodyTrackerControllerDelegate for notifying listeners about VROBodyTrackedState updates.
     */
    std::weak_ptr<VROBodyTrackerControllerDelegate> _delegate;

    /*
     A cache of all filtered ML joints provided by the VROBodyTracker thus far with a
     valid position in 3D space.
     */
    std::map<VROBodyJointType, VROBodyJoint> _cachedTrackedJoints;

    /*
     Final filtered and processed joint positional data on which to apply onto the IKRig.
     */
    std::map<VROBodyJointType, VROVector3f> _cachedModelJoints;

    /*
     The root position and normal of the plane to project onto when using kProjectToPlaneTracking
     depth tests.
     */
    VROVector3f _projectedPlanePosition;
    VROVector3f _projectedPlaneNormal;

    /*
     The rig, skeleton and node associated with the currently bound model.
     */
    std::shared_ptr<VRONode> _modelRootNode;

    /*
     True if this controller is currently calibrating the latest set of ML joints to the IKRig.
     */
    bool _calibrating;

    /*
     Debugging UI components containing debug box nodes representing the locations of
     tracked ML body positions.
     */
    std::shared_ptr<VRONode> _bodyControllerRoot;

#if VRO_PLATFORM_IOS
    VROViewAR *_view;
#endif

    /*
     Process, filter and update this controller's latest known set of _cachedTrackedJoints
     with the latest found ML 2D points given by VROBodyTracker.
     */
    void processJoints(const std::map<VROBodyJointType, VROBodyJoint> &joints);
    void projectJointsInto3DSpace(std::map<VROBodyJointType, VROBodyJoint> &joints);
    void updateCachedJoints(std::map<VROBodyJointType, VROBodyJoint> &joints);

    void calibrateModelToMLTorsoScale();
    VROVector3f getMLRootPosition();
                                     
    /*
     Updates the current VROBodyTrackedState and notifies the attached VROBodyTrackerControllerDelegate.
     */
    void setBodyTrackedState(VROBodyTrackedState state);

    /*
     Updates 3D model's rig with the latest set of known 3D positions
     */
    void notifyOnJointUpdateDelegates();

    /*
     Depth tests for projecting a 2D ML screen coordinate into 3D space.
     */
    bool performUnprojectionToPlane(float x, float y, VROMatrix4f &matOut);

};

#endif /* VROBodyTrackerController_h  */
