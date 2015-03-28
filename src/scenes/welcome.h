#include "ofApp.h"
#include "ofEventUtils.h"

#include "Circle.h"

class Welcome : public ofxScene {

  public:
    Circle circle;

    Welcome() : ofxScene("Welcome") {
    }

    void setup() {
       circle.setup(ofColor(127), ofGetWidth()/2, ofGetHeight()/2, 200, 2.0);
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
