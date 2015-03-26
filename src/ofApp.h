#pragma once
#include "ofMain.h"

#include "ofxScenes.h"

class ofApp : public ofxSceneManagerApp {

  public:
    void setup();
    void update();
    void draw();

    ofxSceneManager sceneManager;
    int lastScene;
};
