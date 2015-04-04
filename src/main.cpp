#include "ofMain.h"
#include "ofApp.hpp"

int main(){
  ofGLWindowSettings glWindowSettings;
  glWindowSettings.setGLVersion(2,1);
  glWindowSettings.windowMode = OF_FULLSCREEN;
  ofCreateWindow(glWindowSettings);
  ofHideCursor();
  ofRunScenesApp(new ofApp());
}
