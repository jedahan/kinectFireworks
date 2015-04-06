#pragma once
#include <ofxScenes.h>
#include <ofEventUtils.h>
#include <ofEvents.h>

#include "../ofApp.hpp"
#include "../Circle.hpp"

class Fireworks : public ofxScene {

  public:
    ofApp * app;
    ofPolyline trail;
    int explode_count;
    Circle x, y, z;

    ofShader shader;
    ofTexture texture;
    ofVboMesh vbo;
    vector<ofPoint> velocities;

    Fireworks() : ofxScene("Fireworks") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
      explode_count = 0;
      float w = ofGetWidth();
      float h = ofGetHeight();
      x.setup(ofColor(0,255,255), 2*w/6, h*2/3, 100, 0.5);
      y.setup(ofColor(255,0,255), 3*w/6, h*2/3, 100, 0.5);
      z.setup(ofColor(255,255,0), 4*w/6, h*2/3, 100, 0.5);
      ofAddListener(x.SELECTED, this, &Fireworks::setColor);
      ofAddListener(y.SELECTED, this, &Fireworks::setColor);
      ofAddListener(z.SELECTED, this, &Fireworks::setColor);

      vbo.clear();
      vbo.setMode(OF_PRIMITIVE_POINTS);
      ofDisableArbTex();
      ofLoadImage(texture,"dot.png");
    }

    void setColor(Circle &c){
      app->selectedColor = c.c;
    }

    void nextScene(int &i){
      app->getSceneManager()->nextScene(true);
    }

    void update() {
      updateTrail();
      if(app->selectedColor != x.c) x.update();
      if(app->selectedColor != y.c) y.update();
      if(app->selectedColor != z.c) z.update();

      for(int i=0; i<vbo.getNumVertices(); i++) {
        velocities[i].x += 0.5-ofRandomuf();
        velocities[i].y += 0.5-ofRandomuf();
        velocities[i].y += 2.0/ofGetFrameRate(); // gravity
        ofPoint p = vbo.getVertex(i) + velocities[i];
        vbo.setVertex(i, p);
      }
    }

    void updateEnter(){
      explode_count=0;
      update();
      finishedEntering();
    }

    void updateExit(){
      update();
      finishedExiting();
    }

    void updateTrail() {
      trail.addVertex(app->mouseX, app->mouseY);

      if(trail.size() > ofGetFrameRate() / 2) {
        ofPolyline tmp;
        for(unsigned int i=1; i<trail.size(); i++ ){
          tmp.addVertex(trail[i]);
        }
        trail = tmp;
        float x0 = trail[0].x, y0 = trail[0].y;
        float x1 = trail[trail.size()-1].x, y1 = trail[trail.size()-1].y;

        if(ofDist(x0,y0,x1,y1) > ofGetWidth()/4) {
          explode(x0,y0,x1,y1);
          trail.clear();
        }
      }
    }

    void draw() {
      ofBackground(0,0,0);

      if(vbo.getNumVertices()){
        ofEnablePointSprites();

        glPointSize(5);
        vbo.drawVertices();

        ofDisablePointSprites();
      }

      switch(explode_count){
        case 0:
          app->drawStringCenter("Swipe your hand quickly");
          break;
        case 1:
          app->drawStringCenter("coool fireworks! do it again!");
          break;
        case 2:
          app->drawStringCenter("POW! alright, you got the hang of this");
          break;
        case 3:
          app->drawStringCenter("Now make your own show!");
          break;
        default:
          app->drawStringCenter("Swipe your hand quickly");
          break;
      }

      trail.draw();

      x.draw();
      y.draw();
      z.draw();
    }

    void explode(int x0, int y0, int x1, int y1){
      explode_count++;
      vbo.clear();
      velocities.clear();
      for(int i=0; i<500; i++){
        vbo.addVertex(ofPoint(x1,y1));
        vbo.addColor(app->selectedColor);
        ofPoint p = ofPoint(ofNormalize(x1-x0, 0, ofGetWidth()), ofNormalize(y1-y0,0,ofGetHeight()));
        p.x += (0.5-ofRandomuf());
        p.y += (0.5-ofRandomuf());
        velocities.push_back(10*p);
      }
    }

};
