/* Your code here! */
#include "maze.h"
#include <time.h>
#include <queue>
#include <algorithm>

using std::queue;
using std::pair;

SquareMaze::SquareMaze(){

}

void SquareMaze::makeMaze(int width, int height){


  width_ = width;
  height_ = height;

  walls_.resize(width);
  for(vector<vector<bool>> & col : walls_){
    col.resize(height);
    for(vector<bool> & w : col){
      //0 for right and 1 for down
    w = {true, true};
    //   for(bool a : w){
    //   std::cout<<a;
    //   }
    //   std::cout<<std::endl;
    }
  }

  DisjointSets set;
  set.addelements(width*height);
  unsigned pieces = width*height;



  while(pieces>1){
    int x = rand() % width;
    int y = rand() % height;
    unsigned rd = rand() % 2;

    if(walls_[x][y][rd]){

      int setindex1 = -1;
      int setindex2 = -1;

      if(rd == 0 && x<width-1){
        setindex1 = y*width + x;
        setindex2 = y*width + x + 1;

      }
      else if(rd==1 && y<height-1){
        setindex1 =  y*width + x;
        setindex2 = (y+1)*width + x;

      }

      if(setindex1>=0 && setindex2 >=0){
        if(set.find(setindex1)!=set.find(setindex2)){
          pieces-=1;
          set.setunion(set.find(setindex1), set.find(setindex2));
          walls_[x][y][rd] = false;

        }
      }

    }

  }

}

bool SquareMaze::canTravel(int x, int y, int dir) const{
    if(dir<2){
      return !walls_[x][y][dir];
    }
    if(dir==2&&x>0){
      return !walls_[x-1][y][0];
    }
    else if(dir == 3 && y>0){
      return !walls_[x][y-1][1];
    }
    return false;

}

void SquareMaze::setWall(int x, int y, int dir, bool exists){
  if(x>=0 && y>=0 && x<width_ && y <height_){
    walls_[x][y][dir] = exists;
  }
}

vector<int> SquareMaze::solveMaze(){
  //remove this when done writing function
  //return vector<int>();

  vector<vector<int>> distances(width_);
  vector<vector<int>> parentdir(width_);
  for(int i = 0; i<width_; i++){
    distances[i].resize(height_);
    std::fill(distances[i].begin(), distances[i].end(), -1);
    parentdir[i].resize(height_);
    std::fill(parentdir[i].begin(), parentdir[i].end(), -1);

  }
  int bottomsleft = width_;
  queue<pair<int, int>> traversal;

  traversal.push({0,0});
  distances[0][0] = 0;

  while(!traversal.empty() && bottomsleft > 0){
    pair<int, int> & next = traversal.front();
    int x = next.first;
    int y = next.second;
    int newdistance = distances[x][y] +1;
    if(canTravel(x, y, 0) && distances[x+1][y]<0){
      distances[x+1][y] = newdistance;
      parentdir[x+1][y] = 0;
      traversal.push({x+1, y});
      if(y == height_-1){
        bottomsleft-=1;
      }
    }
    if(canTravel(x, y, 1) && distances[x][y+1]<0){
      distances[x][y+1] = newdistance;
      parentdir[x][y+1] = 1;
      traversal.push({x, y+1});
      if(y == height_-1){
        bottomsleft-=1;
      }
    }
    if(canTravel(x, y, 2) && distances[x-1][y]<0){
      distances[x-1][y] = newdistance;
      parentdir[x-1][y] = 2;
      traversal.push({x-1, y});
      if(y == height_-1){
        bottomsleft-=1;
      }
    }
    if(canTravel(x, y, 3) && distances[x][y-1]<0){
      distances[x][y-1] = newdistance;
      parentdir[x][y-1] = 3;
      traversal.push({x, y-1});


    }

    traversal.pop();
  }

    // for(auto & x: distances){
    //   for(auto y : x){
    //     std::cout<<y<< " ";
    //   }
    //   std::cout<<std::endl;
    // }

    int maxindex = 0;
    int maxdistance = distances[0][height_-1];
    for(int i = 1; i<width_; i++){
      if(distances[i][height_-1]>maxdistance){
        maxdistance = distances[i][height_-1];
        maxindex = i;

      }
    }


    vector<int> result;
    pair<int, int> currposs = {maxindex, height_-1};
    int currdir = parentdir[currposs.first][currposs.second];
    while(currdir>=0){
      result.push_back(currdir);
      if(currdir==0){
        currposs.first = currposs.first-1;
      }
      else if(currdir==1){
        currposs.second = currposs.second-1;
      }
      else if(currdir==2){
        currposs.first = currposs.first+1;
      }
      else if(currdir==3){
        currposs.second = currposs.second+1;
      }
      currdir = parentdir[currposs.first][currposs.second];

    }
    std::reverse(result.begin(), result.end());

    return result;

  }

PNG* SquareMaze::drawMaze() const{
  int iwidth = width_*10 +1;
  int iheight = height_*10 +1;
  PNG* result = new PNG(iwidth, iheight);
  for(int i = 10; i<iwidth; i++){
    result->getPixel(i,0).l = 0.0;
  }
  for(int i = 0; i<iheight; i++){
    result->getPixel(0,i).l = 0.0;
  }
  for(int x = 0; x < width_; x++){
    for(int y = 0; y < height_; y++){
      if(walls_[x][y][0]){
        for(int k = 0; k<11; k++){
          result->getPixel((x+1)*10,y*10+k).l = 0;
        }
      }
      if(walls_[x][y][1]){
        for(int k = 0; k<11; k++){
          result->getPixel((x)*10+k,(y+1)*10).l = 0;
        }
      }
    }
  }


  return result;
}

PNG* SquareMaze::drawMazeWithSolution(){
  //change this later
  PNG* result = drawMaze();
  vector<int> solution = solveMaze();
  pair<int, int> currpos = {5,5};
  HSLAPixel red(0, 1, 0.5, 1);
  for(int currdir : solution){
    if(currdir==0){
      for(int i = 0; i<11; i++){
        HSLAPixel & change = result->getPixel(currpos.first, currpos.second);
        change = red;
        currpos.first +=1;
      }
      currpos.first-=1;
    }
    if(currdir==1){
      for(int i = 0; i<11; i++){
        HSLAPixel & change = result->getPixel(currpos.first, currpos.second);
        change = red;
        currpos.second +=1;
      }
      currpos.second-=1;
    }

    if(currdir==2){
      for(int i = 0; i<11; i++){
        HSLAPixel & change = result->getPixel(currpos.first, currpos.second);
        change = red;
        currpos.first -=1;
      }
      currpos.first+=1;

    }
    if(currdir==3){
      for(int i = 0; i<11; i++){
        HSLAPixel & change = result->getPixel(currpos.first, currpos.second);
        change = red;
        currpos.second -=1;
      }
      currpos.second+=1;

    }
  }

  pair<int, int> destination = {currpos.first/10, currpos.second/10};
  for(int k = 1; k<10; k++){
    result->getPixel((destination.first)*10+k,(destination.second+1)*10).l = 1;
  }

  return result;


}
