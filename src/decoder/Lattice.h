#ifndef _LATTICE_H_
#define _LATTICE_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_set>
#include "utils/StringUtil.h"
#include "utils/LOG.h"
#define _DEBUG_ 0

using namespace std;


class Node{
 public:
  std::string word;
  std::string read;
  int endpos;
  double score;
  double f;
  double g;
  double h;
  Node *prev;
  Node *next; // for N-Best's backward
  Node(){
    score = 1.0; // init score
  }
  Node(string _word, string _read, int _endpos){
    word = _word;
    read = _read;
    endpos = _endpos;
    score = 1.0; // init score
    //    length = strlen_utf8(read);
    g = 0.0;
    f = 0.0;
    h = 0.0;
    prev = (Node*)NULL;
    next = (Node*)NULL;
  };
  int length(){return strlen_utf8(word);};
  //~Node(){ free(prev);};
  ~Node(){
    //delete prev;
  };
  bool is_bos() const{
    //std::cout << "[debug/jmiyake]" << endpos << std::endl;
    //return (word=="<s>" && endpos == 0) ?  true : false;
    return (word=="<s>") ?  true : false;
    
  };
  bool is_eos() const {
    return (word=="</s>" && endpos !=0) ? true : false;
  };
  bool empty() const{
    return word.empty() ? true : false;
  }
  void set(const std::string& w, const std::string& r, int p){
    word=w;
    read=r;
    endpos=p;
  }
  Node& operator=(const Node& o){
    word=o.word;
    read=o.read;
    endpos=o.endpos;
    score=o.score;
    prev=o.prev;
    next=o.next;
    return *this;
  }
  
  int get_prev_pos(){
    int startpos = 0;
    if(is_eos()){
      startpos = endpos - 2;
    }else if(is_bos()){
      startpos = -1;
    }else{
      startpos = endpos-strlen_utf8(word)-1;
    }
    return startpos;
    
  }

};


typedef Node** Graph;
// Manage 
//   word graph
//   hyp lattice
class Lattice{
 public:
  ;
  //int stateID;
  //std::unordered_map<int, Node> forward; // stateID -> node
  //std::unordered_map<int, Node> backward;
  Node*        operator[](int i)       { return (Node*)_graph[i]; }
  Node* const  operator[](int i) const { return (Node*)_graph[i]; }
  

  void addFrame(int n_frame);
  void expandNode(int n_frame, const std::vector<Node>& nodes);
  void addNode(size_t n_frame, const Node& node);
  
  Lattice(){
    _graph = (Node**)NULL;
    _node_size = (size_t*)NULL;
    _frame_size = 0;
  };
  Lattice(size_t frame_size){
    _graph = (Node**)NULL;
    _node_size = (size_t*)NULL;
    _frame_size = 0;
    resize(frame_size);
  };
  
  void resize(size_t frame_size);
  void clear();

  ~Lattice(){
    clear();
  };

  Lattice& operator=(const Lattice& o){
    _graph = o._graph;
    _frame_size = o._frame_size;
    _node_size = o._node_size;
    return *this;
  }

  int get_prev_pos(Node& node){
    if(node.is_eos()){
      int startpos = node.endpos - 2;
      return startpos;
      
    }else if(node.is_bos()){
      return -1;

    }else{
      int startpos=node.endpos-node.length()-1;
      return startpos;
    }
  };
  size_t sizeOfFrame()const{
    return _frame_size;
  }
  size_t sizeOfNodePerFrame(size_t n_frame)const{
    return _node_size[n_frame];
  }
  void dump();
  
 private:
  ;
  Graph _graph;
  size_t _frame_size;
  size_t *_node_size;

};



#endif

