#pragma once
#include "ofMain.h"

#include "ofxScenes.h"
#include "ofxSimpleTimer.h"

class ofApp : public ofxSceneManagerApp {

  public:
    void setup();
    void start(int &args);
    void update();
    void draw();
    void keyPressed(int key);

    ofxSceneManager sceneManager;
    ofxSimpleTimer timer;
    int lastScene;
};
