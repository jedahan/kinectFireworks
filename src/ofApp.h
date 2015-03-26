#pragma once
#include "ofMain.h"

#include "ofxScenes.h"
#include "ofxSimpleTimer.h"

class ofApp : public ofxScenesApp {

  public:
    void setup();
    void start(int &args);
    void interstitial(int &args);
    void update();
    void draw();
    void keyPressed(int key);

    ofxSceneManager sceneManager;
    ofxSimpleTimer timer;
    int lastScene;
};
