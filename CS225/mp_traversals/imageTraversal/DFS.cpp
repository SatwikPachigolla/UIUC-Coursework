#include <iterator>
#include <cmath>

#include <list>
#include <queue>
#include <stack>
#include <vector>

#include "../cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"
#include "DFS.h"


/**
 * Initializes a depth-first ImageTraversal on a given `png` image,
 * starting at `start`, and with a given `tolerance`.
 *
 * @param png The image this DFS is going to traverse
 * @param start The start point of this DFS
 * @param tolerance If the current point is too different (difference larger than tolerance) with the start point,
 * it will not be included in this DFS
 */
DFS::DFS(const PNG & png, const Point & start, double tolerance) {
  /** @todo [Part 1] */
  start_ = start;
  tolerance_ = tolerance;
  image_ = png;
  s_.push(start);
  vect.resize(png.width());
  for(auto & a:vect){
    a.resize(png.height());
  }
}

/**
 * Returns an iterator for the traversal starting at the first point.
 */
ImageTraversal::Iterator DFS::begin() {
  /** @todo [Part 1] */
  return ImageTraversal::Iterator(*this, start_);
}

/**
 * Returns an iterator for the traversal one past the end of the traversal.
 */
ImageTraversal::Iterator DFS::end() {
  /** @todo [Part 1] */
  return ImageTraversal::Iterator();
}

/**
 * Adds a Point for the traversal to visit at some point in the future.
 */
void DFS::add(const Point & point) {
  /** @todo [Part 1] */
  // unsigned x = point.x;
  // unsigned y = point.y;
  // HSLAPixel & pixel = image_.getPixel(x,y);
  // if((x+1<image_.width()) && (calculateDelta(pixel, image_.getPixel(x+1, y))<tolerance_) && !visited(x+1, y)){
  //   s_.push(Point(x+1, y));
  // }
  // if((y+1<image_.height()) && (calculateDelta(pixel, image_.getPixel(x, y+1))<tolerance_) && !visited(x, y+1)){
  //   s_.push(Point(x, y+1));
  // }
  // if((x>=1) && (calculateDelta(pixel, image_.getPixel(x-1, y))<tolerance_) && !visited(x-1, y)){
  //   s_.push(Point(x-1, y));
  // }
  // if((y>=1) && (calculateDelta(pixel, image_.getPixel(x, y-1))<tolerance_) && !visited(x, y-1)){
  //   s_.push(Point(x, y-1));
  // }
  s_.push(point);
}

/**
 * Removes and returns the current Point in the traversal.
 */
Point DFS::pop() {
  /** @todo [Part 1] */
  Point x = s_.top();
  s_.pop();
  return x;
}

/**
 * Returns the current Point in the traversal.
 */
Point DFS::peek() const {
  /** @todo [Part 1] */
  return s_.top();
}

/**
 * Returns true if the traversal is empty.
 */
bool DFS::empty() const {
  /** @todo [Part 1] */
  return s_.empty();
}
