#include "ofMain.h"
#include "ofApp.h"

int main(){
  ofSetupOpenGL(1024, 768, OF_FULLSCREEN);
  ofHideCursor();
  ofRunScenesApp(new ofApp());
}
