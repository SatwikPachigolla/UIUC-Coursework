/**
 * @file StickerSheet.h
 * Contains your declaration of the interface for the StickerSheet class.
 */
#pragma once

#include "Image.h"

class StickerSheet{
  public:
    StickerSheet(const Image &picture, unsigned max);
    ~StickerSheet();
    StickerSheet(const StickerSheet &other);
    const StickerSheet & operator=(const StickerSheet& other);
    void changeMaxStickers(unsigned max);
    int addSticker(Image &sticker, unsigned x, unsigned y);
    bool translate(unsigned index, unsigned x, unsigned y);
    void removeSticker(unsigned index);
    Image* getSticker(unsigned index);
    Image render() const;
    unsigned max_;

    class Sticker{
      public:
        Sticker(Image inImg, unsigned inX, unsigned inY){
          img = inImg;
          x = inX;
          y = inY;
        }
        Image img;
        unsigned x;
        unsigned y;
    };
    Sticker* base_;

    Sticker** stickers_;
private:
    void destroy();
    void copy(const StickerSheet & other);




};
