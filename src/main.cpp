#include "ofMain.h"
#include "ofApp.h"

int main(){
//  ofAppGlutWindow window;
//  ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
  ofSetupOpenGL(1024, 768, OF_FULLSCREEN);
  ofHideCursor();
  ofRunApp(new ofApp());
}
