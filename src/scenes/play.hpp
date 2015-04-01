#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"
#include "../Circle.hpp"

class Play : public ofxScene {

  public:
    ofApp * app;
    ofPolyline trail;
    vector<ofMesh> fireworks;
    Circle a, b, c;

    Play() : ofxScene("Play") {
      app = (ofApp*) ofxGetAppPtr();
      app->selectedColor = ofColor(255,127,63);
    }

    void setup() {
      int w = ofGetViewportWidth();
      int h = ofGetViewportHeight();

      a.setup(ofColor(0,255,255), 2*w/6, h/2, 100, 1.5);
      ofAddListener(a.SELECTED, this, &Play::selectColor);
      b.setup(ofColor(255,0,255), 3*w/6, h/2, 100, 1.5);
      ofAddListener(b.SELECTED, this, &Play::selectColor);
      c.setup(ofColor(255,255,0), 4*w/6, h/2, 100, 1.5);
      ofAddListener(c.SELECTED, this, &Play::selectColor);
    }

    void selectColor(Circle &c){
      app->selectedColor = c.c;
    }

    void update() {
      updateTrail();
      a.update();
      b.update();
      c.update();
      updateFireworks();
    }

    void updateEnter(){
      update();
      finishedEntering();
    }

    void updateExit(){
      update();
      finishedExiting();
    }

    void updateTrail() {
      trail.addVertex(ofGetMouseX(), ofGetMouseY());

      if(trail.size() > ofGetFrameRate() / 2) {
        ofPolyline tmp;
        for(unsigned int i=1; i<trail.size(); i++ ){
          tmp.addVertex(trail[i]);
        }
        trail = tmp;
        float x0 = trail[0].x, y0 = trail[0].y;
        float x1 = trail[trail.size()-1].x, y1 = trail[trail.size()-1].y;

        if(ofDist(x0,y0,x1,y1) > ofGetWidth()/4) {
          explode(x0,y0,x1,y1);
          trail.clear();
        }
      }

    }

    void updateFireworks() {
      for(auto & firework : fireworks)
        for(auto & vertex : firework.getVertices())
          vertex += ofPoint(100*(0.5-ofRandomuf()),100*(0.5-ofRandomuf()),0);
    }

    void draw() {
      a.draw();
      b.draw();
      c.draw();
      drawFireworks();
      trail.draw();
    }

    void drawFireworks() {
     for(ofMesh firework : fireworks){
       firework.drawVertices();
     }
    }

    void explode(int x0, int y0, int x1, int y1){
      ofMesh firework;
      for(unsigned int i=0; i<100; i++){
        firework.addVertex(ofVec3f(x1+(ofRandom(80)-40),(y1+ofRandom(80)-40),0));
        firework.addColor(ofColor(app->selectedColor));
      }
      // TODO: add a timer to remove a firework in a second or two...
      fireworks.clear();
      fireworks.push_back(firework);
    }

};
