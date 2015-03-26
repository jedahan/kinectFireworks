#include "ofApp.h"

#include "scenes/play.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  ofBackground(0,0,0);

  sceneManager.add(new Play());
  sceneManager.setup();

  ofSetLogLevel("ofxSceneManagerApp", OF_LOG_VERBOSE);

  // for keeping track of the previous scene
  lastScene = sceneManager.getCurrentSceneIndex();

  // automagic management of events
  setSceneManager(&sceneManager);

  // transition to the first scene
  // we use a timer because there seems to be a race condition
  // probably involving setup() not being done yet
  timer.setup(10);
  timer.start(false);
  ofAddListener(timer.TIMER_COMPLETE, this, &ofApp::start);
}

void ofApp::start(int &args) {
  sceneManager.gotoScene("Play", true);
}

void ofApp::update() {
  timer.update();
  // with setSceneManager(&sceneManager), sceneManager.getScene().update() will automagically be run
}

void ofApp::draw() {
  // with setSceneManager(&sceneManager), sceneManager.getScene().draw() will automagically be run
  ofDrawBitmapStringHighlight("#" + ofToString(sceneManager.getCurrentSceneIndex()) + ": " + sceneManager.getCurrentSceneName(), 100, 100);
}

void ofApp::keyPressed(int key) {
  ofLogNotice("ofApp") << "key pressed! " << ofToString(key) << endl;
}
