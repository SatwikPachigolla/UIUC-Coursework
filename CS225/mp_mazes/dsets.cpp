/* Your code here! */
#include "dsets.h"
#include <iostream>

void DisjointSets::addelements(int num)
{
  for(int i = 0; i<num; ++i){
    tree.push_back(-1);
  }
}

int DisjointSets::find(int elem)
{
  if(tree[elem]<0){
    return elem;
  }
  int root =  find(tree[elem]);
  tree[elem] = root;
  return root;
}

void DisjointSets::setunion(int a, int b)
{
  int aroot = find(a);
  int broot = find(b);
  int asize = size(aroot);
  int bsize = size(broot);
  int newsize = -1*(asize+bsize);
  if(bsize>asize)
  {
    tree[aroot] = broot;
    tree[broot] = newsize;
  }
  else
  {
    tree[broot] = aroot;
    tree[aroot] = newsize;
  }
}

int DisjointSets::size(int elem)
{
  int root = find(elem);
  return (-1*tree[root]);

}
