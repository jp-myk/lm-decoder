
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
    prev = nullptr;
    next = nullptr;
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
  //Node*        operator[](int i)       { return (Node*)_graph[i]; }
  //Node* const  operator[](int i) const { return (Node*)_graph[i]; }
  Node* operator[](int i);
  Node* const operator[](int i) const;

  void addFrame(int n_frame);
  void expandNode(int n_frame, const std::vector<Node>& nodes);
  void addNode(size_t n_frame, const Node& node);
  
  Lattice();
  Lattice(size_t frame_size);
  ~Lattice();
  Lattice& operator=(const Lattice& o);
  void resize(size_t frame_size);
  void clear();
  int get_prev_pos(Node& node);
  size_t sizeOfFrame()const;
  size_t sizeOfNodePerFrame(size_t n_frame)const;
  void dump();
  
 private:
  Graph _graph;
  size_t _frame_size;
  size_t *_node_size;

};



#endif

