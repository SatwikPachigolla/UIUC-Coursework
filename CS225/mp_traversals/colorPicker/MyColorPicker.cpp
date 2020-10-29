#include "../cs225/HSLAPixel.h"
#include "../Point.h"

#include "ColorPicker.h"
#include "MyColorPicker.h"

using namespace cs225;

#define PI 3.14159265
/**
 * Picks the color for pixel (x, y).
 * Using your own algorithm
 */
 MyColorPicker::MyColorPicker(Point center)
  : center(center){ }

HSLAPixel MyColorPicker::getColor(unsigned x, unsigned y) {
  double dx = (1.0)*x-(1.0)*center.x;
  double dy = (1.0)*y-(1.0)*center.y;
  double h = 0;
  // if(dx==0){
  //   if(dy<0){
  //     h = 270.0;
  //   }
  //   h = 90.0;
  // }
  // else{
  h = atan2(dx, dy) * 180 /PI + 180.0;

  // }
  return HSLAPixel(h , 1, 0.5);
}
