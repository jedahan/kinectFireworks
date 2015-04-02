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
    ofxSimpleTimer fireworkTimer;

    ofShader shader;
    ofTexture texture;
    ofVbo vbo;
    vector<ofVec2f> points;
    vector<ofVec2f> sizes;
    vector<ofVec2f> velocities;

    Fireworks() : ofxScene("Fireworks") {
      app = (ofApp*) ofxGetAppPtr();
    }

    void setup() {
      ofDisableArbTex();
      ofLoadImage(texture,"dot.png");

      int total=(int)points.size();
      vbo.setVertexData(&points[0], total, GL_DYNAMIC_DRAW);
      vbo.setVertexData(&sizes[0], total, GL_DYNAMIC_DRAW);
#ifdef TARGET_OPENGLES
      shader.load("shaders_gles/shader");
#else
      shader.load("shaders/shader");
#endif
      explode_count = 0;
      float w = ofGetWidth();
      float h = ofGetHeight();
      x.setup(ofColor(0,255,255), 2*w/6, h*2/3, 100, 1.5);
      y.setup(ofColor(255,0,255), 3*w/6, h*2/3, 100, 1.5);
      z.setup(ofColor(255,255,0), 4*w/6, h*2/3, 100, 1.5);
      ofAddListener(x.SELECTED, this, &Fireworks::setColor);
      ofAddListener(y.SELECTED, this, &Fireworks::setColor);
      ofAddListener(z.SELECTED, this, &Fireworks::setColor);
      fireworkTimer.setup(3000);
      ofAddListener(fireworkTimer.TIMER_COMPLETE, this, &Fireworks::popFirework);
    }

    void setColor(Circle &c){
      app->selectedColor = c.c;
    }

    void popFirework(int &i){
      points.clear();
    }

    void nextScene(int &i){
      app->getSceneManager()->nextScene(true);
    }

    void update() {
      updateTrail();
      if(app->selectedColor != x.c) x.update();
      if(app->selectedColor != y.c) y.update();
      if(app->selectedColor != z.c) z.update();
      fireworkTimer.update();
      updateFireworks();
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

    void updateFireworks() {
      for(int i=0; i<points.size(); i++)
        points[i] += ofVec3f(100*(0.5-ofRandomuf()),100*(0.5-ofRandomuf()),0);
    }

    void draw() {
      glDepthMask(GL_FALSE);
      ofSetColor(255,100,90);
      ofEnableBlendMode(OF_BLENDMODE_ADD);
      ofEnablePointSprites();
      shader.begin();
      texture.bind();
      vbo.draw(GL_POINTS, 0, (int)points.size());
      texture.unbind();
      shader.end();
      ofDisablePointSprites();
      ofDisableBlendMode();

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
          app->drawStringCenter("I'll leave you to your own show...");
          break;
        default:
          app->drawStringCenter("I'll leave you to your own show...");
          break;
      }

      trail.draw();

      x.draw();
      y.draw();
      z.draw();
    }

    void explode(int x0, int y0, int x1, int y1){
      explode_count++;
      for(int i=0; i<100; i++){
        points[i].x = x1 + 50*(ofRandomuf()-0.5);
        points[i].y = y1 + 50*(ofRandomuf()-0.5);
        velocities[i].x = ofRandomuf();
        velocities[i].y = ofRandomuf();
      }

      int total = (int)points.size();
      vbo.setVertexData(&points[0], total, GL_DYNAMIC_DRAW);
    }

};
