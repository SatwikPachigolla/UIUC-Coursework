#include <cmath>
#include <iterator>
#include <iostream>

#include "../cs225/HSLAPixel.h"
#include "../cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

/**
 * Calculates a metric for the difference between two pixels, used to
 * calculate if a pixel is within a tolerance.
 *
 * @param p1 First pixel
 * @param p2 Second pixel
 * @return the difference between two HSLAPixels
 */
double ImageTraversal::calculateDelta(const HSLAPixel & p1, const HSLAPixel & p2) {
  double h = fabs(p1.h - p2.h);
  double s = p1.s - p2.s;
  double l = p1.l - p2.l;

  // Handle the case where we found the bigger angle between two hues:
  if (h > 180) { h = 360 - h; }
  h /= 360;

  return sqrt( (h*h) + (s*s) + (l*l) );
}

bool ImageTraversal::visited(unsigned x, unsigned y){
  return vect[x][y];
}

/**
 * Default iterator constructor.
 */
 ImageTraversal::Iterator::Iterator() {
   /** @todo [Part 1] */
   trav= NULL;
 }

ImageTraversal::Iterator::Iterator(ImageTraversal & traversal, const Point start) {
  /** @todo [Part 1] */
  trav= &traversal;
  startPoint = start;
  curr = start;
}

/**
 * Iterator increment opreator.
 *
 * Advances the traversal of the image.
 */
ImageTraversal::Iterator & ImageTraversal::Iterator::operator++() {
  /** @todo [Part 1] */
  unsigned x = curr.x;
  unsigned y = curr.y;
  trav->vect[x][y] = true;
  PNG & pic = trav->image_;
  // trav->add(curr);

  HSLAPixel & pixel = pic.getPixel(trav->start_.x,trav->start_.y);
  if((x+1<pic.width()) && (calculateDelta(pixel, pic.getPixel(x+1, y))<trav->tolerance_) && !trav->visited(x+1, y)){
    trav->add(Point(x+1, y));
  }
  if((y+1<pic.height()) && (calculateDelta(pixel, pic.getPixel(x, y+1))<trav->tolerance_) && !trav->visited(x, y+1)){
    trav->add(Point(x, y+1));
  }
  if((x>=1) && (calculateDelta(pixel, pic.getPixel(x-1, y))<trav->tolerance_) && !trav->visited(x-1, y)){
    trav->add(Point(x-1, y));
  }
  if((y>=1) && (calculateDelta(pixel, pic.getPixel(x, y-1))<trav->tolerance_) && !trav->visited(x, y-1)){
    trav->add(Point(x, y-1));
  }
  if(!trav->empty()){
    curr = trav->peek();
  }
  while(!trav->empty() && trav->vect[curr.x][curr.y]){
    curr = trav->pop();
  }
  return *this;
}

/**
 * Iterator accessor opreator.
 *
 * Accesses the current Point in the ImageTraversal.
 */
Point ImageTraversal::Iterator::operator*() {
  /** @todo [Part 1] */
  return curr;
}

/**
 * Iterator inequality operator.
 *
 * Determines if two iterators are not equal.
 */
bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
  /** @todo [Part 1] */
  if((trav == NULL || trav->empty()) && (other.trav == NULL || other.trav->empty())){
    return false;
  }
  return (trav != other.trav);
}
