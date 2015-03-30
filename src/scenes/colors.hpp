#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"

class Colors : public ofxScene {

  public:
    ofApp * app;
    Circle a, b, c;
    vector<ofColor> chosenColors;
    ofxSimpleTimer timer;

    Colors() : ofxScene("Colors") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
      int w = ofGetViewportWidth();
      int h = ofGetViewportHeight();

      a.setup(ofColor(0,255,255), 2*w/6, h/4*3, 100, 1.5);
      ofAddListener(a.SELECTED, this, &Colors::setColor);
      b.setup(ofColor(255,0,255), 3*w/6, h/4*3, 100, 1.5);
      ofAddListener(b.SELECTED, this, &Colors::setColor);
      c.setup(ofColor(255,255,0), 4*w/6, h/4*3, 100, 1.5);
      ofAddListener(c.SELECTED, this, &Colors::setColor);

      timer.setup(2000);
      ofAddListener(timer.TIMER_COMPLETE, this, &Colors::nextScene);
    }

    void update() {
      a.update();
      b.update();
      c.update();
      timer.update();
    }

    void nextScene(int &i){
      cout << "going to next scene" << endl;
      app->getSceneManager()->nextScene(true);
    }

    void setColor(ofColor &c) {
      bool found = false;
      for(ofColor & color : chosenColors)
        if(color == c) { found = true; }
      if(!found) chosenColors.push_back(c);
      if(chosenColors.size() >= 3) timer.start(false);
      app->selectedColor = c;
    }

    void updateEnter(){
      update();
      finishedEntering();
    }

    void updateExit(){
      update();
      finishedExiting();
    }

    void draw() {
      a.draw();
      b.draw();
      c.draw();
      if(!timer.bIsRunning){
        app->drawStringCenter("Now choose each of the colors ("+ofToString(3-chosenColors.size())+" left)");
      } else {
        app->drawStringCenter("Woohoo! Let's move on...");
      }
    }

};
