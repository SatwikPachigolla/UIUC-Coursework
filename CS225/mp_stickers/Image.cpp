#include "Image.h"
#include <cmath>

using cs225::HSLAPixel;

void Image::lighten(){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.l<.9){
        pixel.l += .1;
      }
      else{
        pixel.l = 1.0;
      }

    }
  }
}

void Image::lighten(double amount){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.l<(1.0-amount)){
        pixel.l += amount;
      }
      else{
        pixel.l = 1.0;
      }

    }
  }
}

void Image::darken(){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.l>.1){
        pixel.l -= .1;
      }
      else{
        pixel.l = 0.0;
      }

    }
  }
}

void Image::darken(double amount){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.l>amount){
        pixel.l -= amount;
      }
      else{
        pixel.l = 0.0;
      }

    }
  }
}

void Image::saturate(){
  for (unsigned x = 0; x < this->width(); x++) {
    for (unsigned y = 0; y < this->height(); y++) {
      HSLAPixel & pixel = this->getPixel(x, y);

      if(pixel.s<.9){
        pixel.s += .1;
      }
      else{
        pixel.s = 1.0;
      }

    }
  }
}

void Image::saturate(double amount){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.s<(1.0-amount)){
        pixel.s += amount;
      }
      else{
        pixel.s = 1.0;
      }

    }
  }
}

void Image::desaturate(){
  for (unsigned x = 0; x < this->width(); x++) {
    for (unsigned y = 0; y < this->height(); y++) {
      HSLAPixel & pixel = this->getPixel(x, y);

      if(pixel.s>.1){
        pixel.s -= .1;
      }
      else{
        pixel.s = 0.0;
      }

    }
  }
}

void Image::desaturate(double amount){
  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.s>amount){
        pixel.s -= amount;
      }
      else{
        pixel.s = 0.0;
      }

    }
  }
}

void Image::grayscale() {

  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      pixel.s = 0;
    }
  }

}

void Image::rotateColor(double degrees) {

  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      pixel.h += degrees;
      while(pixel.h<0.0){
        pixel.h+=360.0;
      }

      pixel.h = fmod(pixel.h, 360);

    }
  }

}

void Image::illinify() {

  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {
      HSLAPixel & pixel = getPixel(x, y);

      if(pixel.h<=113.5 || pixel.h >= 293.5){
        pixel.h = 11;
      }
      else{
        pixel.h = 216;
      }

    }
  }
}

void Image::scale(double factor){

  PNG *old = new PNG(*this);
  this->resize((unsigned)(width()*factor), (unsigned)(height()*factor));

  for (unsigned x = 0; x < width(); x++) {
    for (unsigned y = 0; y < height(); y++) {

      HSLAPixel & oldpixel = old->getPixel(unsigned(x/factor), unsigned(y/factor));
      HSLAPixel & newpixel = getPixel(x,y);
      newpixel.h = oldpixel.h;
      newpixel.s = oldpixel.s;
      newpixel.l = oldpixel.l;
      newpixel.a = oldpixel.a;



    }
  }
  delete old;
}

void Image::scale(unsigned inWidth, unsigned inHeight){
  double factor = std::min(inWidth/(double)width(), inHeight/(double)height());
  scale(factor);
}
