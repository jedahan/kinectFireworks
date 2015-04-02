#include "ofApp.hpp"
#include "scenes/scenes.hpp"

void ofApp::setup(){
  ofGetWindowPtr()->setWindowTitle("fireworks");
  ofEnableSmoothing();
  ofSetCircleResolution(60);
  ofSetVerticalSync(true);
  ofBackground(0,0,0);
  debug = false;
  handFound = debug;

  float radius=ofGetHeight()/20;
  restartCircle.setup(ofColor(ofRandom(15,127),ofRandom(15,127),ofRandom(15,127)), ofGetWidth()-4*radius, ofGetHeight()-4*radius, radius, 2);
  ofAddListener(restartCircle.SELECTED, this, &ofApp::restart);
  setupKinect();

  sceneManager.add(new Welcome());
  sceneManager.add(new Colors());
  sceneManager.add(new Fireworks());
  sceneManager.setup();

  selectedColor = ofColor(127);

  ofSetLogLevel("ofxSceneManagerApp", OF_LOG_VERBOSE);

  lastScene = sceneManager.getCurrentSceneIndex();

  setSceneManager(&sceneManager);

  // transition to the first scene
  // we use a timer because there seems to be a race condition
  // probably involving setup() not being done yet
  timer.setup(500);
  timer.start(false);
  ofAddListener(timer.TIMER_COMPLETE, this, &ofApp::start);

  // this app timeout will reset after a minute
  appTimeout.setup(60*1000);
  appTimeout.start(false);
  ofAddListener(appTimeout.TIMER_COMPLETE, this, &ofApp::start);

  // this hand timeout will hide the cursor if it hasn't moved in 5 seconds
  handTimeout.setup(5000);
  handTimeout.start(false);
  ofAddListener(handTimeout.TIMER_COMPLETE, this, &ofApp::handTimedout);
  // this listener will reset the hand listener if the mouse has moved
  ofAddListener(ofEvents().mouseMoved, this, &ofApp::handMoved);

  ofGetAppPtr()->mouseX = ofGetWidth()/2;
  ofGetAppPtr()->mouseY = ofGetHeight()/2;

  font.load("helveticaneue.ttf", 30);

  cursor.load("cursor.png");
  cursor.setAnchorPercent(0.5,0.5);
  highlight.load("cursor.png");
  highlight.resize(cursor.getWidth()*1.1,cursor.getHeight()*1.15);
  highlight.setAnchorPercent(0.5,0.5);
}

void ofApp::handTimedout(int &i){
  handFound = false;
  appTimeout.start(false);
}

void ofApp::handMoved(ofMouseEventArgs &e){
  appTimeout.reset();
  handTimeout.reset();
  handTimeout.start(false);
  handFound = true;
}
void ofApp::drawStringCenter(string text){
  ofSetColor(ofColor(255,255,255));
  font.drawString(text, (ofGetWidth()-font.stringWidth(text))/2, font.stringHeight(text)*2);
}

void ofApp::restart(Circle &c){
  int a=18;
  start(a);
}

void ofApp::start(int &i){
  sceneManager.gotoScene(sceneManager.getSceneIndex("Welcome"), true);
}

// the scene manager will handle scene.update()
void ofApp::update() {
  timer.update();
  appTimeout.update();
  handTimeout.update();

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

    if(contourFinder.nBlobs > 0){
      float blobx = contourFinder.blobs[0].centroid.x;
      float bloby = contourFinder.blobs[0].centroid.y;

      const float xPercent = ofMap(blobx, kinect.width, 0,  0.0, 1.0);
      const float yPercent = ofMap(bloby, 0, kinect.height, 0.0, 1.0);

      const float xScaledPercent = xPercent*xPercent*(3.0-2.0*xPercent); // easeInOut
      const float yScaledPercent = yPercent*yPercent*(3.0-2.0*yPercent); // easeInOut
      // TODO: switch to ofxAnimatable if this fails hard
      // OR TRY http://cubic-bezier.com/#.42,0,.58,1 ofInterpolateCubic(0.42,0.00,0.58,1.00,percent)...
      ofGetAppPtr()->mouseX = ofLerp(ofGetAppPtr()->mouseX, xScaledPercent * ofGetWidth(), 0.27);
      ofGetAppPtr()->mouseY = ofLerp(ofGetAppPtr()->mouseY, yScaledPercent * ofGetHeight(), 0.26);
      static ofMouseEventArgs mouseEventArgs;
      mouseEventArgs.x = ofGetAppPtr()->mouseX;
      mouseEventArgs.y = ofGetAppPtr()->mouseY;
      ofNotifyEvent(ofEvents().mouseMoved, mouseEventArgs);
    }
  }

  restartCircle.update();
}

// the scene manager will handle scene.draw()
void ofApp::draw() {
  if(sceneManager.getCurrentSceneName()!="Welcome"){
    restartCircle.draw();
    ofSetColor(255,255,255);
    string text="restart";
    font.drawString(text, restartCircle.x-font.stringWidth(text)/2, restartCircle.y+font.stringHeight(text)/2);
  }

  if(handFound || debug){
    ofSetColor(255,255,255);
    highlight.draw(mouseX, mouseY);
    ofSetColor(selectedColor);
    cursor.draw(mouseX,mouseY);
  }

  if(debug){
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
      handFound = debug;
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


