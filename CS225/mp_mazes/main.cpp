#include <iostream>
#include "dsets.h"
#include "maze.h"
#include "cs225/PNG.h"

using namespace std;

int main()
{
    // Write your own main here

    SquareMaze maze1;
  	maze1.makeMaze(50, 50);
    SquareMaze maze2;
  	maze2.makeMaze(50, 50);

    PNG * out1 = maze1.drawMazeWithSolution();


    PNG * out2 = maze2.drawMazeWithSolution();
    HSLAPixel red(0, 1, 0.5, 1);
    out1->resize(991,1001);
    for(unsigned i = 0; i<501; i++){
      for (unsigned j = 0; j<501; j++){
        out1->getPixel(i+490, j+500) = out2->getPixel(i, j);
      }
    }
    for(unsigned i = 0; i<10; i++){
      out1->getPixel(495, 495+i) = red;
    }
    out1->writeToFile("creative" + string(".png"));

    delete out1;
    delete out2;

    return 0;
}
