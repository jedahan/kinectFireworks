#include "ofApp.h"

#define MS_FOR_SELECTION 3000

void ofApp::setup(){
  ofEnableSmoothing();
  ofBackground(0);
  ofGetWindowPtr()->setWindowTitle("fireworks");
  startTime = endTime = -1;

  cursor.load("cursor.png");
  mouseX = ofGetWidth()/2;
  mouseY = ofGetHeight()/2;
  setupCircles();
  setupKinect();
}

void ofApp::setupCircles() {
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

void ofApp::setupKinect() {
  kinect.setRegistration(true);
  kinect.init(false, false); // disable video image (faster fps)
  kinect.open();

  grayImage.allocate(kinect.width, kinect.height);
  grayThreshNear.allocate(kinect.width, kinect.height);
  grayThreshFar.allocate(kinect.width, kinect.height);

  nearThreshold = 230;
  farThreshold = 200;

  ofSetFrameRate(60);

  angle = 0;
  kinect.setCameraTiltAngle(angle);
}

void ofApp::update(){
  line.clear();
  Circle * s = whichCircle();
  if(s){
    ofPoint p = ofPoint(s->x,s->y);
    float r = s->r+20;
    float percent = (ofGetElapsedTimeMillis()-startTime)/MS_FOR_SELECTION;
    if(percent > 360/line.getCircleResolution()/100){
      line.moveTo(p);
      line.arc(p,r,r,270,270+360*percent);
    }
  }
  if(endTime > 0 && ofGetElapsedTimeMillis() >= endTime){
    ofMessage msg(ofToString("selected"));
    ofSendMessage(msg);
    endTime = startTime = -1;
  }
  for( ofMesh firework : fireworks ) {
    ofVec3f * vertices = firework.getVerticesPointer();

    for( int i = 0; i < vertices->length(); i++){
      ofVec3f tmpVertex = firework.getVertex(i);
      tmpVertex.x += 1;
      firework.setVertex(i, tmpVertex);
    }
  }
}

void ofApp::draw(){
  if( endTime > 0)
    line.draw();

  for(Circle c : circles){
    ofSetColor(c.c);
    ofDrawCircle(c.x,c.y,c.r);
  }
  ofSetColor(selectedColor);
  cursor.setAnchorPercent(0.5,0.5);
  cursor.draw(mouseX,mouseY);
  for(ofMesh firework : fireworks){
    firework.drawVertices();
  }
}

void ofApp::keyReleased(int key){

}

void ofApp::mouseMoved(int x, int y){
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
  selectedColor = whichCircle()->c;
}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::updateKinect() {

  ofBackground(100, 100, 100);

  kinect.update();

  // there is a new frame and we are connected
  if(kinect.isFrameNew()) {

    // load grayscale depth image from the kinect source
    grayImage.setFromPixels(kinect.getDepthPixels());

    // we do two thresholds - one for the far plane and one for the near plane
    // we then do a cvAnd to get the pixels which are a union of the two thresholds
    grayThreshNear = grayImage;
    grayThreshFar = grayImage;
    grayThreshNear.threshold(nearThreshold, true);

    grayThreshFar.threshold(farThreshold);
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);

   // update the cv images
   grayImage.flagImageChanged();

  // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
  // also, find holes is set to true so we will get interior contours as well...
    contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
  }

}

void ofApp::drawKinect() {
  ofSetColor(255, 255, 255);
  contourFinder.draw(10, 320, 400, 300);
  ofxCvBlob hand = contourFinder.blobs.at(0);
  mouseX = hand.centroid.x;
  mouseY = hand.centroid.y;

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

  ofDrawBitmapString(reportStream.str(), 20, 652);
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

    case '1':
      kinect.setLed(ofxKinect::LED_GREEN);
      break;

    case '2':
      kinect.setLed(ofxKinect::LED_YELLOW);
      break;

    case '3':
      kinect.setLed(ofxKinect::LED_RED);
      break;

    case '4':
      kinect.setLed(ofxKinect::LED_BLINK_GREEN);
      break;

    case '5':
      kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
      break;

    case '0':
      kinect.setLed(ofxKinect::LED_OFF);
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
