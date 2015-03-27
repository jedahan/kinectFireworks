#include "ofApp.h"
#include "ofEventUtils.h"

#include "Circle.h"

class Welcome : public ofxScene {

  public:
    Circle circle;

    Welcome() : ofxScene("Welcome") {
    }

    void setup() {
       circle.setup(ofColor(255), ofGetWidth()/2, ofGetHeight()/2, 20, 2000);
       ofAddListener(circle.SELECTED, this, &Welcome::nextScene);
    }

    void update() {
      circle.update();
    }

    ofEvent<void> NEXTSCENE;

    void nextScene() {
      ofNotifyEvent(NEXTSCENE);
    }

    void draw() {
      circle.draw();
    }

    void windowResized(int w, int h) {
      circle.x = w/2;
      circle.y = w/2;
    }
};
