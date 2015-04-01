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

    Welcome() : ofxScene("Welcome") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
       circle.setup(ofColor(127, 188, 97), ofGetWidth()/2, ofGetHeight()/2, 200, 2.0);
       ofAddListener(circle.SELECTED, this, &Welcome::nextScene);
    }

    void nextScene(Circle &c) {
      app->selectedColor = c.c;
      app->getSceneManager()->nextScene(true);
    }

    void update() {
      circle.update();
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
      app->drawStringCenter("Please hold out your hand, and hover over the circle to start");
      circle.draw();
    }

    void windowResized(int w, int h) {
      circle.x = w/2;
      circle.y = w/2;
    }
};
