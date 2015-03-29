#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"
#include "../Circle.h"

class Welcome : public ofxScene {

  public:
    Circle circle;
    ofEvent<void> NEXTSCENE;
    ofApp* app;

    Welcome() : ofxScene("Welcome") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
       circle.setup(ofColor(127), ofGetWidth()/2, ofGetHeight()/2, 200, 2.0);
       ofAddListener(circle.SELECTED, this, &Welcome::nextScene);
    }

    void update() {
      circle.update();
    }

    void draw() {
      circle.draw();
      app->drawStringCenter("Please hold up your hand to start");
    }

    void nextScene() {
      ofNotifyEvent(NEXTSCENE);
    }

    void windowResized(int w, int h) {
      circle.x = w/2;
      circle.y = w/2;
    }
};
