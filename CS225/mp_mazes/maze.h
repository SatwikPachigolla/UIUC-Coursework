/* Your code here! */
#pragma once

#include <vector>
#include <iostream>
#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"
#include "dsets.h"

using std::vector;
using cs225::PNG;
using cs225::HSLAPixel;

class SquareMaze{
  public:
    SquareMaze();
    bool canTravel(int x, int y, int dir) const;
    void makeMaze(int width, int height);
    void setWall(int x, int y, int dir, bool exists);
    vector<int> solveMaze();
    PNG* drawMaze() const;
    PNG* drawMazeWithSolution();
  private:
    int width_;
    int height_;
    vector<vector<vector<bool>>> walls_;



};
