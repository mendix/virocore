//
//  VROControllerInputCardboardiOS.cpp
//  ViroRenderer
//
//  Copyright © 2017 Viro Media. All rights reserved.
//
#include "VROInputControllerCardboardiOS.h"

VROVector3f VROInputControllerCardboardiOS::getDragForwardOffset() {
    // on iOS Cardboard 1) we don't have drag 2) since forward vector of
    // the camera matches the "controller" there's no offset anyways.
    return VROVector3f();
}

void VROInputControllerCardboardiOS::onProcess(const VROCamera &camera) {
    updateOrientation(camera);
}

void VROInputControllerCardboardiOS::onScreenClicked(){
    // As ios doesn't have separate down up events, we simulate a button click
    // by triggering both click down / click up.
    VROInputControllerBase::onButtonEvent(ViroCardBoard::ViewerButton, VROEventDelegate::ClickState::ClickDown);
    VROInputControllerBase::onButtonEvent(ViroCardBoard::ViewerButton, VROEventDelegate::ClickState::ClickUp);
}

std::string VROInputControllerCardboardiOS::getHeadset() {
  return std::string("cardboard");
}

std::string VROInputControllerCardboardiOS::getController() {
  return std::string("cardboard");
}

void VROInputControllerCardboardiOS::updateOrientation(const VROCamera &camera){
    // Grab controller orientation
    VROQuaternion rotation = camera.getRotation();
    VROVector3f controllerForward = rotation.getMatrix().multiply(kBaseForward);
    
    // Perform hit test
    VROInputControllerBase::updateHitNode(camera, camera.getPosition(), controllerForward);
    
    // Process orientation and update delegates
    VROInputControllerBase::onMove(ViroCardBoard::InputSource::Controller, camera.getPosition(), rotation, controllerForward);
    VROInputControllerBase::processGazeEvent(ViroCardBoard::InputSource::Controller);
}
