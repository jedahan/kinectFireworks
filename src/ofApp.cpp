#include "ofApp.h"

void ofApp::setup(){
  ofGetWindowPtr()->setWindowTitle("fireworks");
  ofEnableSmoothing();
  ofSetCircleResolution(30);
  ofSetVerticalSync(true);
  ofBackground(0,0,0);
  debug = false;

  setupKinect();

  sceneManager.add(new Welcome());
  sceneManager.add(new Play());
  sceneManager.setup();

  ofSetLogLevel("ofxSceneManagerApp", OF_LOG_VERBOSE);

  lastScene = sceneManager.getCurrentSceneIndex();

  setSceneManager(&sceneManager);

  // transition to the first scene
  // we use a timer because there seems to be a race condition
  // probably involving setup() not being done yet
  timer.setup(500);
  timer.start(false);
  ofAddListener(timer.TIMER_COMPLETE, this, &ofApp::start);

  startTime = endTime = -1;

  ofGetAppPtr()->mouseX = ofGetWidth()/2;
  ofGetAppPtr()->mouseY = ofGetHeight()/2;

  cursor.load("cursor.png");
  cursor.setAnchorPercent(0.5,0.5);
  highlight.load("cursor.png");
  highlight.resize(cursor.getWidth()*1.1,cursor.getHeight()*1.15);
  highlight.setAnchorPercent(0.5,0.5);
}

void ofApp::start(int &args){
  sceneManager.nextScene(true);
  ofAddListener(((Welcome *) (sceneManager.getScene("Welcome")))->NEXTSCENE, this, &ofApp::nextScene);
}

void ofApp::nextScene() {
  sceneManager.nextScene(true);
}

void ofApp::update() {
  timer.update();
  updateKinect();
  // with setSceneManager(&sceneManager), sceneManager.getScene().update() will automagically be run
}

void ofApp::draw() {
  ofSetColor(255,255,255);
  highlight.draw(mouseX, mouseY);
//  ofSetColor(Play::selectedColor);
  ofSetColor(255,0,255);
  cursor.draw(mouseX,mouseY);

  // with setSceneManager(&sceneManager), sceneManager.getScene().draw() will automagically be run
  if(debug){
    ofDrawBitmapStringHighlight("#" + ofToString(sceneManager.getCurrentSceneIndex()) + ": " + sceneManager.getCurrentSceneName(), 100, 100);
    if(timer.bIsRunning){
      ofDrawBitmapStringHighlight(" TIMER IS " + ofToString(timer.getNormalizedProgress()), 10, 10);
    }
    drawKinect();
  }
}

void ofApp::exit() {
  kinect.setCameraTiltAngle(0); // zero the tilt on exit
  kinect.close();
}

void ofApp::setupKinect() {
  kinect.setRegistration(true);
  kinect.init();
  kinect.open();

  grayImage.allocate(kinect.width, kinect.height);
  grayThreshNear.allocate(kinect.width, kinect.height);
  grayThreshFar.allocate(kinect.width, kinect.height);

  nearThreshold = 255;
  farThreshold = 210;

  ofSetFrameRate(60);

  angle = 15;
  kinect.setCameraTiltAngle(angle);
  kinect.setLed(ofxKinect::LED_OFF);
}


void ofApp::updateKinect() {
  kinect.update();
  if(kinect.isFrameNew()) {
    grayImage.setFromPixels(kinect.getDepthPixels());
    grayThreshNear = grayImage;
    grayThreshFar = grayImage;
    grayThreshNear.threshold(nearThreshold, true);
    grayThreshFar.threshold(farThreshold);
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
    grayImage.flagImageChanged();
    contourFinder.findContours(grayImage, 100, (kinect.width*kinect.height)/10, 1, false);

    if(contourFinder.nBlobs > 0) {
      const float xPercent = ofMap(contourFinder.blobs[0].centroid.x, kinect.width, 0, -1.0, 1.0);
      const float yPercent = ofMap(contourFinder.blobs[0].centroid.y, 0, kinect.height,-1.0, 1.0);
      const float xScaledPercent = xPercent*xPercent*(3.0-2.0*xPercent); // easeInOut
      const float yScaledPercent = yPercent*yPercent*(3.0-2.0*yPercent); // easeInOut
      // TODO: switch to ofxAnimatable if this fails hard
      // OR TRY http://cubic-bezier.com/#.42,0,.58,1 ofInterpolateCubic(0.42,0.00,0.58,1.00,percent)...
      ofGetAppPtr()->mouseX = ofLerp(ofGetAppPtr()->mouseX, xScaledPercent * ofGetWidth(), 0.27);
      ofGetAppPtr()->mouseY = ofLerp(ofGetAppPtr()->mouseY, yScaledPercent * ofGetHeight(), 0.26);
      mouseMoved(ofGetAppPtr()->mouseX,ofGetAppPtr()->mouseY);
    }
  }
}


void ofApp::drawKinect() {
  ofSetColor(255, 255, 255);
  float offset = ofGetHeight()-400;
  kinect.draw(10,offset,400,300);
  kinect.drawDepth(410,offset,400,300);
  grayImage.draw(810,offset,400,300);
  contourFinder.draw(1210, offset, 400, 300);

  // draw instructions
  ofSetColor(255, 255, 255);
  stringstream reportStream;

  if(kinect.hasAccelControl()) {
      reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
      << ofToString(kinect.getMksAccel().y, 2) << " / "
      << ofToString(kinect.getMksAccel().z, 2) << endl;
  } else {
      reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
  << "motor / led / accel controls are not currently supported" << endl << endl;
  }

  reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
  << "set near threshold " << nearThreshold << " (press: + -)" << endl
  << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
  << ", fps: " << ofGetFrameRate() << endl
  << "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

  if(kinect.hasCamTiltControl()) {
    reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
      << "press 1-5 & 0 to change the led mode" << endl;
  }

  ofSetColor(255,255,255);
  ofDrawBitmapString(reportStream.str(), 20, 20);
}

void ofApp::keyPressed (int key) {
  switch (key) {
    case 'd':
      debug = !debug;
      break;

    case '>':
    case '.':
      farThreshold ++;
      if (farThreshold > 255) farThreshold = 255;
      break;

    case '<':
    case ',':
      farThreshold --;
      if (farThreshold < 0) farThreshold = 0;
      break;

    case '+':
    case '=':
      nearThreshold ++;
      if (nearThreshold > 255) nearThreshold = 255;
      break;

    case '-':
      nearThreshold --;
      if (nearThreshold < 0) nearThreshold = 0;
      break;

    case 'w':
      kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
      break;

    case 'o':
      kinect.setCameraTiltAngle(angle); // go back to prev tilt
      kinect.open();
      break;

    case 'c':
      kinect.setCameraTiltAngle(0); // zero the tilt
      kinect.close();
      break;

    case OF_KEY_UP:
      angle++;
      if(angle>30) angle=30;
      kinect.setCameraTiltAngle(angle);
      break;

    case OF_KEY_DOWN:
      angle--;
      if(angle<-30) angle=-30;
      kinect.setCameraTiltAngle(angle);
      break;
  }
}
