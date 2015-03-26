#include "play.h"

#define MS_FOR_SELECTION 3000
#define DEBUG false

void Play::setup() {
  ofEnableSmoothing();
  ofGetWindowPtr()->setWindowTitle("fireworks");
  startTime = endTime = -1;

  ofGetAppPtr()->mouseX = ofGetWidth()/2;
  ofGetAppPtr()->mouseY = ofGetHeight()/2;

  cursor.load("cursor.png");
  cursor.setAnchorPercent(0.5,0.5);
  highlight.load("cursor.png");
  highlight.resize(cursor.getWidth()*1.1,cursor.getHeight()*1.15);
  highlight.setAnchorPercent(0.5,0.5);
  setupCircles();
  setupKinect();
  ofLogNotice("Play") << "setup complete " << ofToString(selectedColor) << endl;
}

void Play::setupCircles() {
  int w = ofGetViewportWidth();
  int h = ofGetViewportHeight();
  ofSetCircleResolution(180);
  circles.push_back(Circle(ofColor(0,255,255), 2 * w/6, h/2, 100));
  circles.push_back(Circle(ofColor(255,0,255), 3 * w/6, h/2, 100));
  circles.push_back(Circle(ofColor(255,255,0), 4 * w/6, h/2, 100));
  selectedColor = circles[0].c;
  line.setStrokeWidth(1);
  line.setCircleResolution(180);
}

void Play::setupKinect() {
  kinect.setRegistration(true);
  kinect.init();
  kinect.open();

  grayImage.allocate(kinect.width, kinect.height);
  grayThreshNear.allocate(kinect.width, kinect.height);
  grayThreshFar.allocate(kinect.width, kinect.height);

  nearThreshold = 255;
  farThreshold = 230;

  ofSetFrameRate(30);

  angle = 20;
  kinect.setCameraTiltAngle(angle);
  kinect.setLed(ofxKinect::LED_OFF);
}

void Play::update() {
  updateTrail();
  updateKinect();
  updateCircles();
  updateFireworks();
}

void Play::updateEnter(){
  ofLogNotice("Play") << "update enter start" << endl;
  update();
  finishedEntering();
  ofLogNotice("Play") << "update enter done" << endl;
}

void Play::updateExit(){
  ofLogNotice("Play") << "update exit start" << endl;
  update();
  finishedExiting();
  ofLogNotice("Play") << "update exit done" << endl;
}

void Play::updateTrail() {
  trail.addVertex(ofGetMouseX(), ofGetMouseY());

  if(trail.size() > ofGetFrameRate() / 2) {
    ofPolyline tmp;
    for( int i=1; i<trail.size(); i++ ){
      tmp.addVertex(trail[i]);
    }
    trail = tmp;
    float x0 = trail[0].x, y0 = trail[0].y;
    float x1 = trail[trail.size()-1].x, y1 = trail[trail.size()-1].y;

    if(ofDist(x0,y0,x1,y1) > ofGetWidth()/6) {
      explode(x0,y0,x1,y1);
      trail.clear();
    }
  }

}

void Play::updateKinect() {
  kinect.update();
  if(kinect.isFrameNew()){
    grayImage.setFromPixels(kinect.getDepthPixels());
    grayThreshNear = grayImage;
    grayThreshFar = grayImage;
    grayThreshNear.threshold(nearThreshold, true);
    grayThreshFar.threshold(farThreshold);
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
    grayImage.flagImageChanged();
    contourFinder.findContours(grayImage, 100, (kinect.width*kinect.height)/10, 1, false);

    if(contourFinder.nBlobs > 0) {
      const float w = kinect.width;
      const float h = kinect.height;
      const float newX = ofMap(contourFinder.blobs[0].centroid.x, 0, w, ofGetWidth(), 0);
      const float newY = ofMap(contourFinder.blobs[0].centroid.y, 0, h, 0, ofGetHeight());
      ofGetAppPtr()->mouseX = ofLerp(ofGetAppPtr()->mouseX, newX, 0.09);
      ofGetAppPtr()->mouseY = ofLerp(ofGetAppPtr()->mouseY, newY, 0.09);
      mouseMoved(ofGetAppPtr()->mouseX,ofGetAppPtr()->mouseY);
    }
  }
}

void Play::updateCircles() {
  line.clear();
  Circle * s = whichCircle();
  if(s && s->c != selectedColor){
    ofPoint p = ofPoint(s->x,s->y);
    float r = s->r+20;
    float percent = (ofGetElapsedTimeMillis()-startTime)/MS_FOR_SELECTION;
    if(percent > 360/line.getCircleResolution()/100){
      line.moveTo(p);
      line.arc(p,r,r,270,270+360*percent);
    }
  }
  if(endTime > 0 && ofGetElapsedTimeMillis() >= endTime){
    selectedColor = whichCircle()->c;
    endTime = startTime = -1;
  }
}

void Play::updateFireworks() {
  for(int i=0; i< fireworks.size(); i++) {
    for(int j=0; j<fireworks[i].getNumVertices(); j++){
      fireworks[i].getVertices()[j] += ofPoint(100*(0.5-ofRandomuf()),100*(0.5-ofRandomuf()),0);
    }
  }
}

void Play::draw() {
  drawCircles();
  drawFireworks();
  if(DEBUG) drawKinect();
  trail.draw();
  ofSetColor(255,255,255);
  highlight.draw(ofGetAppPtr()->mouseX,ofGetAppPtr()->mouseY);
  ofSetColor(selectedColor);
  cursor.draw(ofGetAppPtr()->mouseX,ofGetAppPtr()->mouseY);
}

void Play::drawCircles() {
  if( endTime > 0)
    line.draw();

  for(Circle c : circles){
    ofSetColor(c.c);
    ofDrawCircle(c.x,c.y,c.r);
  }
}

void Play::drawFireworks() {
 for(ofMesh firework : fireworks){
   firework.drawVertices();
   firework.drawWireframe();
 }
}

void Play::keyReleased(int key){

}

void Play::checkForSwipe(){

}

void Play::mouseMoved(int x, int y){
  if(whichCircle() && startTime < 0) {
    startTime = ofGetElapsedTimeMillis();
    endTime = startTime + MS_FOR_SELECTION;
  }
  if(! whichCircle() && endTime > 0){
    startTime = endTime = -1;
  }
}

void Play::mouseDragged(int x, int y, int button){
}

Circle * Play::whichCircle() {
  for(int i=0; i < (int)circles.size(); i++){
    if(pointInCircle(ofGetAppPtr()->mouseX,ofGetAppPtr()->mouseY,circles[i])){
      return &circles[i];
    }
  }
  return NULL;
}

bool Play::pointInCircle(int pX, int pY, Circle c) {
    int dx = pX - c.x;
    int dy = pY - c.y;
    return dx*dx + dy*dy <= c.r*c.r;
}

void Play::explode(int x0, int y0, int x1, int y1){
  ofMesh firework;
  for(int i=0; i<100; i++){
    firework.addVertex(ofVec3f(x1+(ofRandom(80)-40),(y1+ofRandom(80)-40),0));
    firework.addColor(ofColor(selectedColor));
  }
  fireworks.push_back(firework);
}

void Play::mousePressed(int x, int y, int button){
  if (!whichCircle()) {
    explode(x,y,ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
  }
}


void Play::mouseReleased(int x, int y, int button){
}

void Play::windowResized(int w, int h){
   circles[0].x = 2 * w/6;
   circles[0].y = h/2;
   circles[1].x = 3 * w/6;
   circles[1].y = h/2;
   circles[2].x = 4 * w/6;
   circles[2].y = h/2;
}

void Play::gotMessage(ofMessage msg) {
}

void Play::dragEvent(ofDragInfo dragInfo){ 

}


void Play::drawKinect() {
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

void Play::exit() {
  kinect.setCameraTiltAngle(0); // zero the tilt on exit
  kinect.close();
}

void Play::keyPressed (int key) {
  switch (key) {
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
