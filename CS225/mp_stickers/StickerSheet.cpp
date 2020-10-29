#include "StickerSheet.h"

using cs225::HSLAPixel;

StickerSheet::StickerSheet(const Image & picture, unsigned max){
  max_ = max;
  stickers_ = new Sticker*[max_];
  for(unsigned i = 0; i<max_; i++){
    stickers_[i] = NULL;
  }
  // Image inter = picture;
  // addSticker(inter, 0, 0);

  base_ = new StickerSheet::Sticker(picture, 0, 0);

}

StickerSheet::~StickerSheet(){
  destroy();
}

void StickerSheet::destroy(){
  for(unsigned i = 0; i<max_; i++){
    delete stickers_[i];
  }
  delete [] stickers_;
  delete base_;
}

StickerSheet::StickerSheet(const StickerSheet & other){
  copy(other);
}

void StickerSheet::copy(const StickerSheet & other){
  base_ = new Sticker(other.base_->img, 0, 0);
  max_ = other.max_;
  stickers_ = new Sticker*[max_];
  for(unsigned i = 0; i<max_; i++){
    if(other.stickers_[i]==NULL){
      stickers_[i] = NULL;
    }
    else{
    stickers_[i] = new StickerSheet::Sticker(
      other.stickers_[i]->img,
      other.stickers_[i]->x, other.stickers_[i]->y);
    }
  }
}

int StickerSheet::addSticker(Image & Sticker, unsigned x, unsigned y){
  for(unsigned i = 0; i<max_; i++){
    if(stickers_[i] == NULL){
      stickers_[i] = new StickerSheet::Sticker(Sticker, x, y);
      return i;
    }
  }
  return -1;

}

void StickerSheet::changeMaxStickers(unsigned max){
  Sticker** temp = new Sticker*[max];
  for(unsigned i = 0; i<max||i<max_; i++){
    if(i<max_&&i<max){
      temp[i] = stickers_[i];
    }
    else if(i<max_){
      delete stickers_[i];
    }
    else{
      temp[i] = NULL;
    }
  }
  Sticker** holder;
  holder = stickers_;
  stickers_ = temp;
  delete [] holder;
  max_ = max;
}

Image* StickerSheet::getSticker(unsigned index){
  if(index>=max_){
    return NULL;
  }
  return &stickers_[index]->img;
}

const StickerSheet & StickerSheet::operator=(const StickerSheet & other){
  if (this != &other) {
    destroy();
    copy(other);
  }
  return *this;
}

void StickerSheet::removeSticker(unsigned index){
  if(index<max_ && stickers_[index] != NULL){
    delete stickers_[index];
    stickers_[index] = NULL;
  }
}

Image StickerSheet::render() const{
  unsigned baseW = base_->img.width();
  unsigned totWidth = baseW;
  unsigned baseH = base_->img.height();
  unsigned totHeight = baseH;
  for(unsigned i = 0; i<max_; i++){
    if(stickers_[i]!=NULL){
      Image current = stickers_[i]->img;
      unsigned w = current.width() + stickers_[i]->x;
      unsigned h = current.height() + stickers_[i]->y;
      if(w>totWidth){
        totWidth = w;
      }
      if(h>totHeight){
        totHeight = h;
      }
    }
  }

  Image result = Image();
  result.resize(totWidth, totHeight);

  Image baseCurr = base_->img;
  for(unsigned j = 0; j<baseW; j++){
    for(unsigned k = 0; k<baseH; k++){
      HSLAPixel & newpixel = baseCurr.getPixel(j,k);
      if(newpixel.a != 0){
        result.getPixel(j, k) = newpixel;
      }
    }

  }

  for(unsigned i = 0; i<max_; i++){
    if(stickers_[i]!=NULL){
      Image current = stickers_[i]->img;
      unsigned x = stickers_[i]->x;
      unsigned y = stickers_[i]->y;
      unsigned w = current.width();
      unsigned h = current.height();
      for(unsigned j = 0; j<w; j++){
        for(unsigned k = 0; k<h; k++){
          HSLAPixel & newpixel = current.getPixel(j,k);
          if(newpixel.a != 0){
            result.getPixel(x+j, y+k) = newpixel;
          }
        }

      }
    }
  }
  return result;
}

bool StickerSheet::translate(unsigned index, unsigned x, unsigned y){
  if(index>=max_ || stickers_[index] == NULL){
    return false;
  }
  Sticker* current = stickers_[index];
  current->x = x;
  current->y = y;
  return true;
}
