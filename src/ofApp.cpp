#include "ofApp.h"

void ofApp::setup(){
  sceneManager.addScene(shared_ptr<ofxScene>(new Play));
  sceneManager.run();
}

void ofApp::update() {
  sceneManager.update();
}

void ofApp::draw() {
  sceneManager.draw();
}
