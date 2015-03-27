#pragma once
#include "ofMain.h"
#include "ofEventUtils.h"

class Circle {

  public:
    int x;
    int y;
    int r;
    int timeout;

    string name;
    ofColor c;
    Circle(){}

    void setup(ofColor _c, int _x, int _y, int _r, int _timeout) {
      c = _c;
      x = _x;
      y = _y;
      r = _r;
      percent = 0.0;
      timeout = _timeout;
      startTime = endTime = -1;
      line.setStrokeWidth(1);
      line.setCircleResolution(60);
    }

    ofPath line;
    ofEvent<void> SELECTED;

    void update() {
      percent = (ofGetElapsedTimeMillis() - startTime)/timeout;
      if(endTime > 0 && ofGetElapsedTimeMillis() >= endTime) {
        ofNotifyEvent(SELECTED); // stole off of ofxSimpleTimer
        startTime = endTime = -1;
      }
      line.clear();
      if(percent > 360/line.getCircleResolution()/100){
        line.moveTo(x,y);
        line.arc(x,y,r,r,270,270+360*percent);
      }
    }

    void draw() {
      if(endTime>0){
        ofSetColor(255);
        line.draw();
      }

      ofSetColor(c);
      ofDrawCircle(x,y,r);
    }

    bool hit(int pX, int pY) {
      int dx = pX - x;
      int dy = pY - y;
      return dx*dx + dy*dy <= r*r;
    }

    int startTime, endTime;
    float percent;

    void mouseMoved(int x, int y){
      if(hit(x,y) && startTime < 0) {
        startTime = ofGetElapsedTimeMillis();
        endTime = startTime + timeout;
      }
      if(! hit(x,y) && endTime > 0){
        startTime = endTime = -1;
      }
    }
};
