#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"

#include <cmath>
#include <string>
#include <cstdlib>

using namespace cs225;

void rotate(std::string inputFile, std::string outputFile) {
  PNG* original = new PNG();
  original->readFromFile(inputFile);
  unsigned width = original->width();
  unsigned height = original->height();

  PNG* image = new PNG(width, height);
  for(unsigned int i = 0; i<width; i++){
    for(unsigned int j = 0; j<height; j++){
      image->getPixel(i,j) = original->getPixel(width - i - 1, height - j - 1);
    }
  }
  image->writeToFile(outputFile);

  delete image;
  delete original;
}

cs225::PNG myArt(unsigned int width, unsigned int height) {
  cs225::PNG png(width, height);
  // TODO: Part 3
  for(unsigned int i = 0; i<width; i++){
    for(unsigned int j = 0; j<height; j++){
      png.getPixel(i,j).l = 0;
    }
  }
  unsigned int filled = 0;
  while(filled<width*height/5){
    int randT = rand()%360;
    int randR = rand()%(int)(pow(pow(width, 2)+pow(height, 2),0.5)) + 16;
    int randW = rand()%(randR/3) + 3;
    for(int t = 0; t<360; t++){
      for(int s = -randW; s < randW; s++){
        unsigned int i = (unsigned int) ((randR + s * cos((t+randT)*M_PI /360.0) ) * cos(t+randT) );
        unsigned int j = (unsigned int)((randR + s * cos((t+randT)*M_PI /360.0)) * sin(t+randT));
        if(i>0 && j>0 && i<width && j<height){
          HSLAPixel & current = png.getPixel((int)i,(int)j);
          if(current.l == 0){
            filled++;
            current.l = 0.5;
            current.a = pow((randW - s)/(randW*2.0), .25);
            current.s = 1;
            current.h = t;
        }
        }
      }
    }



  }




  return png;
}
