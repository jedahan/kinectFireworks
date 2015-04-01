#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"
#include "../Circle.hpp"

class Welcome : public ofxScene {

  public:
    Circle circle;
    ofApp* app;
    unsigned int handSeconds;

    Welcome() : ofxScene("Welcome") {
      app = (ofApp*) ofxGetAppPtr();
      handSeconds = 0;
    }

    void setup() {
       circle.setup(ofColor(127, 188, 97), ofGetWidth()/2, ofGetHeight()/2, 200, 2.0, false);
       ofAddListener(circle.SELECTED, this, &Welcome::nextScene);
    }

    void nextScene(Circle &c) {
      app->selectedColor = c.c;
      app->nextScene();
    }

    void update() {
      circle.update();
      if(app->handFound){
        handSeconds++;
      } else {
        handSeconds = 0;
      }
      if(handSeconds > 2 * ofGetFrameRate()){
        circle.enabled = true;
      }
    }

    void updateExit(){
      update();
      finishedExiting();
    }

    void updateEnter(){
      update();
      finishedEntering();
    }

    void draw() {
      if(circle.enabled){
        app->drawStringCenter("Great!, now hold it over the circle to continue");
        circle.draw();
      } else {
        app->drawStringCenter("Please hold up your hand to start");
      }
    }

    void windowResized(int w, int h) {
      circle.x = w/2;
      circle.y = w/2;
    }
};
