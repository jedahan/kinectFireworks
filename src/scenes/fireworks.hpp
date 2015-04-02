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
    vector<ofVec3f> points;
    vector<ofVec3f> sizes;
    vector<ofVec3f> vels;

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
      fireworkTimer.setup(3000);
      ofAddListener(fireworkTimer.TIMER_COMPLETE, this, &Fireworks::popFirework);
      ofDisableArbTex();
      ofLoadImage(texture,"dot.png");
      updateVbo();
#ifdef TARGET_OPENGLES
      shader.load("shaders_gles/shader");
#else
      shader.load("shaders/shader");
#endif

    }

    void setColor(Circle &c){
      app->selectedColor = c.c;
    }

    void popFirework(int &i){
      cout << "popping firework" << endl;
      points.clear();
      sizes.clear();
      updateVbo();
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
      /*
      for(int i=0; i<points.size(); i++)
        points[i] += vels[i];
      */
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

    void updateVbo(){
      int total = (int)points.size();
      vbo.setVertexData(&points[0], total, GL_DYNAMIC_DRAW);
      vbo.setNormalData(&sizes[0], total, GL_DYNAMIC_DRAW);
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
      glDepthMask(GL_FALSE);
      ofSetColor(app->selectedColor);
      ofEnableBlendMode(OF_BLENDMODE_ADD);
      ofEnablePointSprites();
      shader.begin();
      texture.bind();
      vbo.draw(GL_POINTS, 0, (int)points.size());
      texture.unbind();
      shader.end();
      ofDisablePointSprites();
      ofDisableBlendMode();
      glDepthMask(GL_TRUE);

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

      for(int i=0; i<100; i++){
        addPoint(x1, y1, x1-x0 + ofRandomuf(), y1-y0 + ofRandomuf());
      }

      updateVbo();
    }

    void addPoint(float x, float y, float vx, float vy) {
      ofVec3f p = ofVec3f(x, y, 0.5);
      p *= 400;
      points.push_back(p);

      float size = ofRandom(5, 50);
      sizes.push_back(ofVec3f(size));

      ofVec3f v(vx, vy, 0);
      vels.push_back(v);
    }

};
