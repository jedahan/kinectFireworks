#pragma once
#include "ofMain.h"
#include "ofEventUtils.h"

class Circle {

  public:
    int x;
    int y;
    int r;
    float timeout;

    float startTime, endTime;
    float percent;

    string name;
    ofColor c;
    Circle(){}

    void setup(ofColor _c, int _x, int _y, int _r, float _timeout) {
      c = _c;
      x = _x;
      y = _y;
      r = _r;
      percent = 0.0;
      timeout = _timeout;
      startTime = endTime = -1;
      line.setStrokeWidth(1);
      line.setCircleResolution(60);
      ofAddListener(ofEvents().mouseMoved, this, &Circle::mouseMoved);
    }

    ofPath line;
    ofEvent<void> SELECTED;

    void update() {
      if(startTime > 0){
        percent = (ofGetElapsedTimef() - startTime)/timeout;
        if(percent >= 1.0){
          startTime = endTime = -1;
          ofNotifyEvent(SELECTED);
        }
        line.clear();
        if(percent > 360/line.getCircleResolution()/100.0){
          line.moveTo(x,y);
          line.arc(x,y,r+20,r+20,270,270+360*percent);
        }
      }
    }

    void draw() {
      if(startTime > 0){
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

    void mouseMoved(ofMouseEventArgs& mouse){
      if(startTime > 0 && !(hit(mouse.x,mouse.y))){
        startTime = endTime = -1;
      }
      if(startTime < 0 && hit(mouse.x,mouse.y)) {
        startTime = ofGetElapsedTimef();
        endTime = startTime + timeout;
      }
    }
};
