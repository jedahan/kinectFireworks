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
    void nextScene();
    void setup();
    void start(int &args);
    void interstitial(int &args);
    void update();
    void draw();
    void keyPressed(int key);
    void exit();
    void drawStringCenter(string text);

    ofxSceneManager sceneManager;
    ofxSimpleTimer timer;
    int lastScene;
    bool handFound;
    ofColor selectedColor;

    void setupKinect();
    void updateKinect();
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

    float startTime;
    float endTime;

    bool debug;
};