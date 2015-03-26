#include "ofApp.h"

#include "scenes/play.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  ofBackground(0,0,0);

  sceneManager.add(new Play());
  sceneManager.setup();

  ofSetLogLevel("ofxSceneManagerApp", OF_LOG_VERBOSE);

  // transition to the first scene immediately
  sceneManager.gotoScene("Play", true);

  // for keeping track of the previous scene
  lastScene = sceneManager.getCurrentSceneIndex();

  // automagic management of events
  setSceneManager(&sceneManager);
}

void ofApp::update() {
  // because we setSceneManager, this happens magically
}

void ofApp::draw() {
  // because we setSceneManager, this happens magically
  ofDrawBitmapStringHighlight("#" + ofToString(sceneManager.getCurrentSceneIndex()) + ": " + sceneManager.getCurrentSceneName(), 100, 100);
}
