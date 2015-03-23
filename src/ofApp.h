#pragma once
#include "ofMain.h"

#pragma once
#include "Circle.h"

#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCvBlob.h"

class ofApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();

    void explode(int x, int y);
    void setupKinect();
    void drawKinect();
    void updateKinect();
    void setupCircles();

    vector<Circle> circles;
    bool pointInCircle(int pX, int pY, Circle c);
    Circle * whichCircle();
    ofColor selectedColor;
    ofPath line;
    vector<ofMesh> fireworks;

    bool select;
    float startTime;
    float endTime;

    ofxKinect kinect;

    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image

    ofxCvContourFinder contourFinder;

    ofImage cursor;

    int nearThreshold;
    int farThreshold;

    int angle;
};
