#pragma once
#include "ofMain.h"

class Circle {

	public:
    int x;
    int y;
    int r;
    ofColor c;
    Circle(ofColor _c, int _x, int _y, int _r) {
      c = _c;
      x = _x;
      y = _y;
      r = _r;
    }
};
