/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"
//#include "cs225/RGB_HSL.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */

     size_t rows = theSource.getRows();
     size_t cols = theSource.getColumns();
     MosaicCanvas* result = new MosaicCanvas(rows, cols);

     vector<Point<3>> tilePoints;
     tilePoints.resize(theTiles.size());
     map<Point<3>, size_t> tileMap;

     for(size_t i = 0; i<theTiles.size(); i++){
        tilePoints[i] = convertToXYZ(theTiles[i].getAverageColor());
        tileMap[tilePoints[i]] = i;
     }

     KDTree<3> ourTree(tilePoints);

     for(size_t i = 0; i<rows; i++){
       for(size_t j = 0; j<cols; j++){
         Point<3> nearest = ourTree.findNearestNeighbor(convertToXYZ(theSource.getRegionColor(i,j)));
         result->setTile(i,j, &theTiles[tileMap[nearest]]);
       }
     }

    return result;
}
