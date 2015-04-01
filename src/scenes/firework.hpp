#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"
#include "../Circle.hpp"

class Firework : public ofxScene {

  public:
    ofApp * app;
    ofPolyline trail;
    vector<ofMesh> fireworks;

    Firework() : ofxScene("Firework") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
    }

    void update() {
      updateTrail();
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
      app->drawStringCenter("Swipe your hand quickly");
      for(auto & firework : fireworks)
        firework.drawVertices();
      trail.draw();
    }

    void explode(int x0, int y0, int x1, int y1){
      ofMesh firework;
      app->selectedColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
      for(unsigned int i=0; i<200; i++){
        firework.addVertex(ofVec3f(x1+(ofRandom(80)-40),(y1+ofRandom(80)-40),0));
        firework.addColor(app->selectedColor);
      }
      fireworks.clear();
      fireworks.push_back(firework);
    }

};
