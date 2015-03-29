#include "ofMain.h"
#include "ofApp.hpp"

int main(){
  ofSetupOpenGL(1024, 768, OF_FULLSCREEN);
  ofHideCursor();
  ofRunScenesApp(new ofApp());
}
