#include "Circle.h"

#include "ofxScene.h"

#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCvBlob.h"

class Play : public ofxScene {

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

    void explode(int x0, int y0, int x1, int y1);
    void checkForSwipe();

    void setupKinect();
    void updateKinect();
    void drawKinect();

    void setupCircles();
    void updateCircles();
    void drawCircles();

    void setupFireworks();
    void updateFireworks();
    void drawFireworks();

    void updateTrail();

    ofPolyline trail;
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
    ofImage highlight;

    int nearThreshold;
    int farThreshold;

    int angle;
};
