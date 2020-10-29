/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>
#include <cmath>
#include "kdtree.h"
#include "point.h"
#include <vector>

using namespace std;

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    /**
     * @todo Implement this function!
     */
  if (first[curDim] < second[curDim])
    return true;
  if (second[curDim] < first[curDim])
    return false;
  return (first < second);
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    /**
     * @todo Implement this function!
     */

  double pot_dist = 0;
  double curBest_dist = 0;
  for (int i = 0; i < Dim; i++){
    pot_dist += ((target[i]-potential[i])*(target[i]-potential[i]));
    curBest_dist += ((target[i]-currentBest[i])*(target[i]-currentBest[i]));
  }
  if (pot_dist < curBest_dist)
    return true;
  if (curBest_dist < pot_dist)
    return false;
  return (potential < currentBest);

}

template <int Dim>
Point<Dim> KDTree<Dim>::quickSelect(const vector<Point<Dim>>& curPoints, int curDim, int nthSmallest){

  if (curPoints.size() == 1){
    return curPoints[0];
  }

  int pivot = rand() % curPoints.size(); 

  //std::cout << "PIVOT INDX:" << pivot << std::endl;

  //std::cout << "AFTER2" << std::endl;

  Point<Dim> pivotP = curPoints[pivot];

  vector<Point<Dim>> lowVals;
  vector<Point<Dim>> highVals;
  vector<Point<Dim>> pivots;

  //std::cout << "quicksleect 1" << std::endl;

  for (int i=0; i < int(curPoints.size()); i++){
    Point<Dim> currPoint = curPoints[i];

    if (smallerDimVal(currPoint, pivotP, curDim)){
      lowVals.push_back(currPoint);
    }

    //    if (curPoint[curDim] < pivotP[curDim]){
    //lowVals.push_back(curPoint);
      //std::cout << "LOWPUSH:" << curPoint[curDim] << std::endl;
    //}
    //else if (curPoint[curDim] > pivotP[curDim]){
    //  highVals.push_back(curPoint);
      //std::cout << "HIGHPUSH:" << curPoint[curDim] << std::endl;
    //}

    else if (smallerDimVal(pivotP, currPoint, curDim)){
	highVals.push_back(currPoint);
      }
      //else if (curPoint < pivotP){
      //lowVals.push_back(curPoint);
      //std::cout << "LOWPUSH:" << curPoint[curDim] << std::endl;
      //}
    else{
      pivots.push_back(currPoint);
      //std::cout << "PIVOTPUSH:" << curPoint[curDim] << std::endl;
    }
  }
  

  //std::cout << "after loop" << std::endl;

  //std::cout << "FIRST NTHSMALLEST:" << nthSmallest << std::endl;

  if (nthSmallest < int(lowVals.size())){
    return quickSelect(lowVals, curDim, nthSmallest);
  }

  else if (nthSmallest < int(lowVals.size() + pivots.size())){
    return pivots[0];
  }


  else {
    int newNthSmallest = nthSmallest - lowVals.size() - 1;
    //std::cout << "nthSmallest:" << newNthSmallest << std::endl;
    return quickSelect(highVals, curDim, newNthSmallest);
  }
}


template <int Dim>
Point<Dim> KDTree<Dim>::findMedian(const vector<Point<Dim>>& curPoints, int curDim){
  int nthSmallest = floor((curPoints.size()-1)/2);

  //std::cout << "Before quickselect" << std::endl;
  return quickSelect(curPoints, curDim, nthSmallest);
}

template <int Dim>
vector<Point<Dim>> KDTree<Dim>::partitionLeft(const vector<Point<Dim>>& curPoints, Point<Dim> median, int curDim){
  vector<Point<Dim>> out;
  for (int i=0; i < int(curPoints.size()); i++){
    Point<Dim> curPoint = curPoints[i];
    if (curPoint[curDim] < median[curDim]){
      out.push_back(curPoint);
    }
    if (curPoint[curDim] == median[curDim] && curPoint < median){
      out.push_back(curPoint);
    }
  }
  return out;
}

template <int Dim>
vector<Point<Dim>> KDTree<Dim>::partitionRight(const vector<Point<Dim>>& curPoints, Point<Dim> median, int curDim){
  vector<Point<Dim>> out;
  for (int i=0; i < int(curPoints.size()); i++){
    Point<Dim> curPoint = curPoints[i];
    if (curPoint[curDim] > median[curDim]){
      out.push_back(curPoint);
    }
    if (curPoint[curDim] == median[curDim] && median < curPoint){
      out.push_back(curPoint);
    }
  }
  return out;
}

template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::createKDTree(const vector<Point<Dim>>& curPoints, int curDim){
  int nextDim = (curDim+1)%Dim;

  if (curPoints.size() == 0){
    return NULL;
  }

  //std::cout << "BeforeMedian" << std::endl;

  Point<Dim> median = findMedian(curPoints, curDim);
  typename KDTree<Dim>::KDTreeNode* curr = new typename KDTree<Dim>::KDTreeNode(median);


  //std::cout << "createKDTest" << std::endl;

  vector<Point<Dim>> left = partitionLeft(curPoints, median, curDim);
  vector<Point<Dim>> right = partitionRight(curPoints, median, curDim);

  //std::cout << "partition" << std::endl;

  curr->left = createKDTree(left, nextDim);

  //std::cout << "left" << std::endl;
  curr->right = createKDTree(right, nextDim);
  //std::cout << "right" << std::endl;
  return curr;
}


template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    /**
     * @todo Implement this function!
     */

  if (newPoints.size() == 0){
    root = NULL;
    return;
  }
  root = createKDTree(newPoints, 0);

}


template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::copy(const KDTreeNode* subRoot)
{
  if (subRoot == NULL)
    return NULL;

  //  std::cout << "GOT HERE" << std::endl;
  KDTreeNode* newNode = new KDTreeNode(subRoot->point);

  newNode->left = copy(subRoot->left);
  newNode->right = copy(subRoot->right);
  //std::cout << "aaa" << std::endl;
  return newNode;
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
  /**
   * @todo Implement this function!
   */
  if (other.root == NULL)
    root = NULL;
  else{
    root = copy(other.root);
    size = other.size;
  }
}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
  /**
   * @todo Implement this function!
   */
  if (this != &rhs){
    clear(root);
    root = copy(rhs.root);
    size = rhs.size;
  }
  return *this;
}

template <int Dim>
void KDTree<Dim>::clear(KDTreeNode* subRoot){
  if (subRoot == NULL)
    return;
  clear(subRoot->left);
  clear(subRoot->right);
  delete subRoot;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
  clear(root);
}


template <int Dim>
double distance(Point<Dim> first, Point<Dim> second){
  double dist = 0;
  for (int i = 0; i < Dim; i++){
    dist += ((first[i]-second[i])*(first[i]-second[i]));
  }
  return dist;
}

template <int Dim>
Point <Dim> KDTree<Dim>::neighborHelper(Point<Dim> currBest, KDTreeNode* curr, const Point<Dim>& target, int curDim) const {

  int nextDim = (curDim+1)%Dim;
  Point<Dim> currPoint = curr->point;
  Point<Dim> pot;

  KDTreeNode* first_search;
  bool left = false;


  if (curr == NULL){
    return NULL;
  }
	       
  if (curr->left == NULL && curr->right == NULL){
      currBest = curr->point;
      return currBest;
  }

  else {
    if (smallerDimVal(target, currPoint, curDim)){
      if (curr->left != NULL){
	first_search = curr->left;
	left = true;
      }
      else
	first_search = curr->right;
    }
    else{
      if (curr->right != NULL){
	first_search = curr->right;
      }
      else{
	left = true;
	first_search = curr->left;
      }
    }
  }
   
  currBest = neighborHelper(currBest, first_search, target, nextDim);
  double currBestDist = (double)distance(currBest, target);
  double childDist = (double)pow((currPoint[curDim]-target[curDim]), 2);

  if (shouldReplace(target, currBest, currPoint))
    currBest = currPoint;

  if (left && curr->right && (childDist <= currBestDist)){
    pot = neighborHelper(currBest, curr->right, target, nextDim);
    if (shouldReplace(target, currBest, pot)){
      currBest = pot;
    }
  }
  else{
    if (curr->left && (childDist <= currBestDist)){
      pot = neighborHelper(currBest, curr->left, target, nextDim);
      if (shouldReplace(target, currBest, pot)){
	currBest = pot;
      }
    }
  }
  return currBest;    
}
template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    /**
     * @todo Implement this function!
     */  
  return neighborHelper(root->point, root, query, 0);
}

