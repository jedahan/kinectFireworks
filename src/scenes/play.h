#include "../Circle.h"

#include <ofxScenes.h>

class Play : public ofxScene {

  public:
    Play() : ofxScene("Play") {
      selectedColor = ofColor(255,127,63);
    }

    void setup() {
      int w = ofGetViewportWidth();
      int h = ofGetViewportHeight();

      Circle a, b, c;
      a.setup(ofColor(0,255,255), 2*w/6, h/2, 100, 1500);
      b.setup(ofColor(255,0,255), 3*w/6, h/2, 100, 1500);
      c.setup(ofColor(255,255,0), 4*w/6, h/2, 100, 1500);
      circles.push_back(a);
      circles.push_back(b);
      circles.push_back(c);

      selectedColor = a.c;
      ofLogNotice("Play") << "setup complete " << ofToString(selectedColor) << endl;
    }

    void update() {
      updateTrail();
      for(Circle circle : circles){
        circle.update();
      }
      updateFireworks();
    }

    void updateEnter(){
      ofLogNotice("Play") << "update enter start" << endl;
      update();
      finishedEntering();
      ofLogNotice("Play") << "update enter done" << endl;
    }

    void updateExit(){
      ofLogNotice("Play") << "update exit start" << endl;
      update();
      finishedExiting();
      ofLogNotice("Play") << "update exit done" << endl;
    }

    void updateTrail() {
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

    void updateFireworks() {
      for(int i=0; i< fireworks.size(); i++) {
        for(int j=0; j<fireworks[i].getNumVertices(); j++){
          fireworks[i].getVertices()[j] += ofPoint(100*(0.5-ofRandomuf()),100*(0.5-ofRandomuf()),0);
        }
      }
    }

    void draw() {
      for(Circle circle : circles){
        circle.draw();
      }
      drawFireworks();
      trail.draw();
   }

    void drawFireworks() {
     for(ofMesh firework : fireworks){
       firework.drawVertices();
       firework.drawWireframe();
     }
    }

    void mouseMoved(int x, int y){
      for(Circle circle : circles){
        circle.mouseMoved(x,y);
      }
    }

    void explode(int x0, int y0, int x1, int y1){
      ofMesh firework;
      for(int i=0; i<100; i++){
        firework.addVertex(ofVec3f(x1+(ofRandom(80)-40),(y1+ofRandom(80)-40),0));
        firework.addColor(ofColor(selectedColor));
      }
      fireworks.push_back(firework);
    }

    void windowResized(int w, int h){
      for(int i=0; i<circles.size(); i++){
        circles[i].x = (i+2)*w/6;
        circles[i].y = h/2;
      }
    }

    ofPolyline trail;
    vector<Circle> circles;

    ofColor selectedColor;
    ofPath line;
    vector<ofMesh> fireworks;

    bool select;
};
