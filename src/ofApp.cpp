#include "ofApp.h"

#define MS_FOR_SELECTION 3000

void ofApp::setup(){
  int w = ofGetViewportWidth();
  int h = ofGetViewportHeight();
  ofEnableSmoothing();
  ofSetCircleResolution(180);
  circles.push_back(Circle(ofColor(0,255,255), 2 * w/6, h/2, 100));
  circles.push_back(Circle(ofColor(255,0,255), 3 * w/6, h/2, 100));
  circles.push_back(Circle(ofColor(255,255,0), 4 * w/6, h/2, 100));
  selectedColor = circles[0].c;
  ofBackground(0);
  ofGetWindowPtr()->setWindowTitle("fireworks");
  startTime = endTime = -1;
  line.setStrokeWidth(1);
  line.setCircleResolution(180);
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
  }
  for( ofMesh firework : fireworks ) {
    ofVec3f * vertices = firework.getVerticesPointer();

    for( int i = 0; i < vertices->length(); i++){
      firework.getVertex(i)  += ofRandom(5);
    }
  }
}

void ofApp::draw(){
  line.draw();

  for(Circle c : circles){
    ofSetColor(c.c);
    ofDrawCircle(c.x,c.y,c.r);
  }
  ofSetColor(selectedColor);
  ofDrawRectangle(mouseX-10,mouseY-10,20,20);
 
  for(ofMesh firework : fireworks){
    firework.drawVertices();
  }
}

void ofApp::keyPressed(int key){

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
