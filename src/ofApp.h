#pragma once
#include "ofMain.h"

#include "ofxSceneManager.h"

#include "scenes/play.h"

class ofApp : public ofBaseApp {

  public:
    void setup();
    void update();
    void draw();

    ofxSceneManager sceneManager;
};
