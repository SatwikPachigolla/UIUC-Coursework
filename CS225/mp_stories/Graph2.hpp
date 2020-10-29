#include <queue>
#include <algorithm>
#include <string>
#include <list>

using std::string;
using std::list;
using std::queue;


/**
 * Returns an std::list of vertex keys that creates any shortest path between `start` and `end`.
 *
 * This list MUST include the key of the `start` vertex as the first vertex in the list, the key of
 * the `end` vertex as the last element in the list, and an ordered list of all vertices that must
 * be traveled along the shortest path.
 *
 * For example, the path a -> c -> e returns a list with three elements: "a", "c", "e".
 *
 * You should use undirected edges. Hint: There are no edge weights in the Graph.
 *
 * @param start The key for the starting vertex.
 * @param end   The key for the ending vertex.
 */
template <class V, class E>
list<string> Graph<V,E>::shortestPath(const string start, const string end) {
  // TODO: Part 3
  // std::cout<<"start: "<<start<<" "<<"end: "<<end<<" "<<std::endl;
  // if(vertexMap.find(start)!=vertexMap.end() && start != end && vertexMap.find(end) != vertexMap.end()){
  //
  //   std::cout<<"huh"<<std::endl;
  //   return list<string>();
  // }
  std::unordered_map<std::string, std::string> predecessor;
  queue<std::string> queue;
  queue.push(start);
  bool endfound = false;
  while(!queue.empty() && !endfound){
    string curr = queue.front();
    queue.pop();
    const list<std::reference_wrapper<E>> edges = incidentEdges(curr);
    for(typename list<std::reference_wrapper<E>>::const_iterator it = edges.begin(); it!= edges.end(); ++it){
      string next;
      if((*it).get().source()==curr){
        next = (*it).get().dest().key();
      }
      else{
        next = (*it).get().source().key();
      }
      if(predecessor.find(next)==predecessor.end()){
        predecessor[next] = curr;
        queue.push(next);
      }
      if(next==end){
        endfound = true;
        break;
      }
    }

  }
  // for(std::pair<std::string, std::string> pair : predecessor){
  //   std::cout<<pair.first<<": "<<pair.second<<std::endl;
  // }
  // std::cout<<"queue: ";
  // while(!queue.empty()){
  //   std::cout<<queue.front()<<" ";
  //   queue.pop();
  // }
  // std::cout<<std::endl;

  if(queue.back()!=end){
    // std::cout<<"doesn't work?"<<std::endl;
    return list<string>();

  }

  string curr = end;
  list<string> path;
  path.emplace_front(end);
  while(curr!=start){
    curr = predecessor[curr];
    path.emplace_front(curr);
  }




  // std::cout<<"works?"<<std::endl;

  return path;
}
