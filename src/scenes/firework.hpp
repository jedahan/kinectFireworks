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
    int explode_count;
    ofxSimpleTimer timer;

    Firework() : ofxScene("Firework") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
      explode_count = 0;
      timer.setup(2000);
      ofAddListener(timer.TIMER_COMPLETE, this, &Firework::nextScene);
    }

    void nextScene(int &i){
      app->getSceneManager()->nextScene(true);
    }

    void update() {
      updateTrail();
      timer.update();
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
      switch(explode_count){
        case 0:
          app->drawStringCenter("Swipe your hand quickly");
          break;
        case 1:
          app->drawStringCenter("coool, now swipe " + ofToString(4-explode_count) + " more times");
          break;
        case 2:
          app->drawStringCenter("POW! I wanna see " + ofToString(4-explode_count) + " more fireworks!");
          break;
        case 3:
          app->drawStringCenter("WOOHA! Just one more before we start the main show..");
          break;
        default:
          app->drawStringCenter("Alright, time to play");
          timer.start(false);
          break;
      }

      for(auto & firework : fireworks)
        firework.drawVertices();
      trail.draw();
    }

    void explode(int x0, int y0, int x1, int y1){
      explode_count++;
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
