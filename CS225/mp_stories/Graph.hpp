#include "Graph.h"
#include "Edge.h"
#include "Vertex.h"

#include <string>
#include <iostream>

/**
* @return The number of vertices in the Graph
*/
template <class V, class E>
unsigned int Graph<V,E>::numVertices() const {
  // TODO: Part 2
  return vertexMap.size();
}


/**
* The degree of the vertex. For directed: Sum of in-degree and out-degree
* @return Returns the degree of a given vertex.
* @param v Given vertex to return degree.
*/
template <class V, class E>
unsigned int Graph<V,E>::degree(const V & v) const {
  // TODO: Part 2

  // return degree(v.key());
  typename std::unordered_map<std::string, std::list<edgeListIter>>::const_iterator it1 = adjList.find(v.key());

  if(it1 != adjList.end()){
    return (*it1).second.size();
  }
  return 0;
}


/**
* Inserts a Vertex into the Graph.
* @param key The key of the Vertex to insert
* @return The inserted Vertex
*/
template <class V, class E>
V & Graph<V,E>::insertVertex(std::string key) {
  // TODO: Part 2
  V & v = *(new V(key));
  V_byRef vRef = v;
  vertexMap.insert({key, vRef});
  adjList[key] = std::list<edgeListIter>();
  return v;
}


/**
* Removes a given Vertex
* @param v The Vertex to remove
*/
template <class V, class E>
void Graph<V,E>::removeVertex(const std::string & key) {
  // TODO: Part 2
   std::list<std::reference_wrapper<E>> edges = incidentEdges(key);

  for(auto edge : edges){
    removeEdge(edge.get().source(), edge.get().dest());
  }

  vertexMap.erase(key);
  // std::unordered_map<std::string, std::list<edgeListIter>>::iterator it = adjList.at(key);
  // if(it != adjList.end()){
  //   return;
  // }

}


/**
* Inserts an Edge into the Graph.
* @param v1 The source Vertex
* @param v2 The destination Vertex
* @return The inserted Edge
*/
template <class V, class E>
E & Graph<V,E>::insertEdge(const V & v1, const V & v2) {
  // TODO: Part 2
  E & e = *(new E(v1, v2));
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it = adjList.find(v1.key());
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it2 = adjList.find(v2.key());
  if(it!=adjList.end() && it2!= adjList.end()){
    edgeList.emplace_front(e);
    (*it).second.emplace_front(edgeList.begin());
    (*it2).second.emplace_front(edgeList.begin());
  }
  return e;
}


/**
* Removes an Edge from the Graph. Consider both the undirected and directed cases.
* min(degree(key1), degree(key2))
* @param key1 The key of the source Vertex
* @param key2 The key of the destination Vertex
*/
template <class V, class E>
void Graph<V,E>::removeEdge(const std::string key1, const std::string key2) {
  // TODO: Part 2
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it1 = adjList.find(key1);
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it2 = adjList.find(key2);


  if(it1!=adjList.end() && it2!= adjList.end()){

    typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator itV = degree(key2) < degree(key1) ? it2 : it1;

    for(typename std::list<edgeListIter>::iterator it = (*itV).second.begin(); it!=(*itV).second.end(); ++it){
      if((*(*it)).get().source() == key1 && (*(*it)).get().dest() == key2){
        edgeList.erase(*it);
        if(itV == it1){
          (*it2).second.remove(*it);
        }
        else{
          (*it1).second.remove(*it);
        }
        (*itV).second.erase(it);



        return;
      }
    }



  }

}


/**
* Removes an Edge from the Graph given by the location of the given iterator into the edge list.
* @param it An iterator at the location of the Edge that
* you would like to remove
*/
template <class V, class E>
void Graph<V,E>::removeEdge(const edgeListIter & it) {
  // TODO: Part 2
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it1 = adjList.find((*it).source_);
  typename std::unordered_map<std::string, std::list<edgeListIter>>::iterator it2 = adjList.find((*it).dest_);


  if(it1!=adjList.end() && it2!= adjList.end()){
    (*it1).second.remove(it);
    (*it2).second.remove(it);
    edgeList.erase(it);
  }
}


/**
* Return the list of incident edges from a given vertex.
* For the directed case, consider all edges that has the vertex as either a source or destination.
* @param key The key of the given vertex
* @return The list edges (by reference) that are adjacent to the given vertex
*/
template <class V, class E>
const std::list<std::reference_wrapper<E>> Graph<V,E>::incidentEdges(const std::string key) const {
  // TODO: Part 2
  std::list<std::reference_wrapper<E>> edges;
  // typename std::list<edgeListIter>::iterator it = adjList.find(key);
  // if(it!=adjList.end()){
  //   for(edgeListIter x : adjList.at(it)){
  //     edges.push_back(*x);
  //   }
  // }
  typename std::unordered_map<std::string, std::list<edgeListIter>>::const_iterator it1 = adjList.find(key);

  if(it1 != adjList.end()){
    for(const edgeListIter & it : (*it1).second){
      edges.emplace_front(*it);
    }
  }



  return edges;
}



/**
* Return whether the two vertices are adjacent to one another. Consider both the undirected and directed cases.
* When the graph is directed, v1 and v2 are only adjacent if there is an edge from v1 to v2.
* @param key1 The key of the source Vertex
* @param key2 The key of the destination Vertex
* @return True if v1 is adjacent to v2, False otherwise
*/
template <class V, class E>
bool Graph<V,E>::isAdjacent(const std::string key1, const std::string key2) const {
  // TODO: Part 2
  std::string vert = degree(key2) < degree(key1) ? key2 : key1;
  auto edges = incidentEdges(vert);
  Edge toFind = Edge(key1, key2);
  for(auto edge : edges){
    if(edge.get() == toFind){
      return true;
    }
  }



  return false;
}
