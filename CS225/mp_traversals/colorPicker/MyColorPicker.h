#pragma once

#include "ColorPicker.h"
#include "../cs225/HSLAPixel.h"
#include "../Point.h"
#include <cmath>

using namespace cs225;

/**
 * A color picker class using your own color picking algorithm
 */
class MyColorPicker : public ColorPicker {
public:
  MyColorPicker(Point center);
  HSLAPixel getColor(unsigned x, unsigned y);

private:
  Point center;
};
