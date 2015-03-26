#include "ofApp.h"

#include "scenes/play.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  ofBackground(0);

  sceneManager.addScene(new Play());
  // setup all the scenes immediately instead of on the fly
  sceneManager.setup(true);

  // transition to the first scene immediately
  sceneManager.goToScene("Play", true);

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
  ofDrawBitmapStringHighlight("#" + sceneManager.getCurrentSceneIndex() + ": " + sceneManager.getCurrentSceneName());
}
