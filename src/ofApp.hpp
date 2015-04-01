#pragma once
#include "ofMain.h"
#include "ofEvents.h"
#include "ofEventUtils.h"
#include "Circle.hpp"

#include "ofxScenes.h"
#include "ofxSimpleTimer.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"

class ofApp : public ofxScenesApp {

  public:
    void setup();
    void start(int &args);
    void update();
    void draw();
    void keyPressed(int key);
    void exit();
    void drawStringCenter(string text);
    void appTimedout(int &i);
    void handTimedout(int &i);
    void handMoved(ofMouseEventArgs &e);

    ofxSceneManager sceneManager;
    ofxSimpleTimer timer;
    int lastScene;
    bool handFound;
    ofColor selectedColor;
    ofxSimpleTimer handTimeout;
    ofxSimpleTimer appTimeout;

    void setupKinect();
    void drawKinect();

    ofxKinect kinect;

    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image

    ofxCvContourFinder contourFinder;

    ofImage cursor;
    ofImage highlight;

    ofTrueTypeFont font;

    int nearThreshold;
    int farThreshold;

    int angle;

    bool debug;
};
