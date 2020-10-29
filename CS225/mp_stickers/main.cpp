#include "Image.h"
#include "StickerSheet.h"

int main() {

  //
  // Reminder:
  //   Before exiting main, save your creation to disk as myImage.png
  //
  Image base; base.readFromFile("base.png");

  Image spurs; spurs.readFromFile("spurs.png");

  Image warriors; warriors.readFromFile("warriors.png");
  base.scale(1.25);
  spurs.scale(.68);
  warriors.scale(.50);

  StickerSheet sheet(base, 2);
  sheet.addSticker(spurs, 610, 5);
  sheet.addSticker(warriors, 1120, 475);

  Image result = sheet.render();

  result.writeToFile("myImage.png");

  return 0;
}
