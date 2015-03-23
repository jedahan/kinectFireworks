#include "ofApp.h"

#define MS_FOR_SELECTION 3000
#define DEBUG false

void ofApp::setup(){
  ofEnableSmoothing();
  ofBackground(0);
  ofGetWindowPtr()->setWindowTitle("fireworks");
  startTime = endTime = -1;

  mouseX = ofGetWidth()/2;
  mouseY = ofGetHeight()/2;
  cursor.load("cursor.png");
  cursor.setAnchorPercent(0.5,0.5);
  setupCircles();
  setupKinect();
}

void ofApp::setupCircles() {
  int w = ofGetViewportWidth();
  int h = ofGetViewportHeight();
  ofSetCircleResolution(180);
  circles.push_back(Circle(ofColor(0,255,255), 2 * w/6, 1/5*h, 100));
  circles.push_back(Circle(ofColor(255,0,255), 3 * w/6, 1/5*h, 100));
  circles.push_back(Circle(ofColor(255,255,0), 4 * w/6, 1/5*h, 100));
  selectedColor = circles[0].c;
  line.setStrokeWidth(1);
  line.setCircleResolution(180);
}

void ofApp::setupKinect() {
  kinect.setRegistration(true);
  kinect.init();
  kinect.open();

  grayImage.allocate(kinect.width, kinect.height);
  grayThreshNear.allocate(kinect.width, kinect.height);
  grayThreshFar.allocate(kinect.width, kinect.height);

  nearThreshold = 255;
  farThreshold = 230;

  ofSetFrameRate(30);

  angle = 15;
  kinect.setCameraTiltAngle(angle);
  kinect.setLed(ofxKinect::LED_OFF);
}

void ofApp::update(){
  updateKinect();
  updateCircles();
  updateFireworks();
}

void ofApp::updateKinect() {
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
      const float newX = ofMap(contourFinder.blobs[0].centroid.x, 1/5 * w, 4/5 * w, ofGetWidth(), 0);
      const float newY = ofMap(contourFinder.blobs[0].centroid.y, 1/5 * h, 4/5 * h, 0, ofGetHeight());     
      mouseX = ofLerp(mouseX, newX, 0.09);
      mouseY = ofLerp(mouseY, newY, 0.09);
      mouseMoved(mouseX, mouseY);
    }
  }
}

void ofApp::updateCircles() {
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

void ofApp::updateFireworks() {
  for( ofMesh firework : fireworks ) {
    ofVec3f * vertices = firework.getVerticesPointer();

    for( int i = 0; i < vertices->length(); i++){
      ofVec3f tmpVertex = firework.getVertex(i);
      tmpVertex.x += 1;
      firework.setVertex(i, tmpVertex);
    }
  }
}

void ofApp::draw() {
  drawCircles();
  drawFireworks();
  if(DEBUG) drawKinect();
  ofSetColor(selectedColor);
  cursor.draw(mouseX,mouseY);
}

void ofApp::drawCircles() {
  if( endTime > 0)
    line.draw();

  for(Circle c : circles){
    ofSetColor(c.c);
    ofDrawCircle(c.x,c.y,c.r);
  }
}

void ofApp::drawFireworks() {
 for(ofMesh firework : fireworks){
    firework.drawVertices();
  }
}

void ofApp::keyReleased(int key){

}

void ofApp::checkForSwipe(){
  if(points.size() > 30){
    const ofPoint p0 = points[points.size()-31];
    const ofPoint p1 = points[points.size()-1];
    if(ofDistSquared(p0.x,p0.y,p1.x,p1.y)>100){
	    cout << "xplode"<<endl;
    //  explode(p1.x,p1.y);
    }
  }
}

void ofApp::mouseMoved(int x, int y){
  points.push_back(ofPoint(x,y));
  checkForSwipe();
  if(whichCircle() && startTime < 0) {
    startTime = ofGetElapsedTimeMillis();
    endTime = startTime + MS_FOR_SELECTION;
  }
  if(! whichCircle() && endTime > 0){
    startTime = endTime = -1;
  }
}

void ofApp::mouseDragged(int x, int y, int button){
}

Circle * ofApp::whichCircle() {
  for(int i=0; i < (int)circles.size(); i++){
    if(pointInCircle(mouseX,mouseY,circles[i])){
      return &circles[i];
    }
  }
  return NULL;
}

bool ofApp::pointInCircle(int pX, int pY, Circle c) {
    int dx = pX - c.x;
    int dy = pY - c.y;
    return dx*dx + dy*dy <= c.r*c.r;
}

void ofApp::explode(int x, int y){
  ofMesh firework;
  for(int i=0; i<1000; i++){
    firework.addVertex(ofVec3f(x+(ofRandom(80)-40),(y+ofRandom(80)-40),0));
    firework.addColor(ofColor(selectedColor));
  }
  fireworks.push_back(firework);
}

void ofApp::mousePressed(int x, int y, int button){
  if (!whichCircle()) {
    explode(x,y);
  }
}


void ofApp::mouseReleased(int x, int y, int button){
}

void ofApp::windowResized(int w, int h){
   circles[0].x = 2 * w/6;
   circles[0].y = h/2;
   circles[1].x = 3 * w/6;
   circles[1].y = h/2;
   circles[2].x = 4 * w/6;
   circles[2].y = h/2;
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

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

void ofApp::exit() {
  kinect.setCameraTiltAngle(0); // zero the tilt on exit
  kinect.close();
}

void ofApp::keyPressed (int key) {
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
