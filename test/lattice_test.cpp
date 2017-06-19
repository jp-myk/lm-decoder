#include "decoder/Lattice.h"
#include <cstdio>

void test1(){
  Lattice lattice;
  for(int i=0;i<10;i++){
    lattice.addFrame(i);
    std::vector<Node> nodes;
    Node node1("東京","とうきょう",2);
    Node node2("奈良","なら",4);
    nodes.push_back(node1);
    nodes.push_back(node2);
    lattice.expandNode(i,nodes);
  }
  std::cout << "for end" << std::endl;
  Node node1 = (Node)lattice[0][0];
  std::cout << "node1.word=" << node1.word << std::endl;
  Node node2 = (Node)lattice[0][1];
  std::cout << "node2.word=" << node2.word << std::endl;

  Node *nodes = (Node*)lattice[0];
  //printf("size=%d\n", (int)(sizeof(nodes)/sizeof(nodes[0])));
  for(size_t i=0;i<lattice.sizeOfNodePerFrame(0);i++){
    std::cout << "nodes[0][" << i << "]=" << nodes[i].word << std::endl;
  }
  lattice.dump();

}

void test2(){
  int lattice_size = 1;
  Lattice lattice(lattice_size);
  
  for(int i=0;i<lattice_size;i++){
    Node node1("東京","とうきょう",2);
    Node node2("奈良","なら",4);
    lattice.addNode(i,node1);
    lattice.addNode(i,node2);
  }
  
  std::cout << "for end" << std::endl;
  Node node1 = (Node)lattice[0][0];
  std::cout << "node1.word=" << node1.word << std::endl;
  Node node2 = (Node)lattice[0][1];
  std::cout << "node2.word=" << node2.word << std::endl;
  Node *nodes = (Node*)lattice[0];
  //printf("size=%d\n", (int)(sizeof(nodes)/sizeof(nodes[0])));
  for(size_t i=0;i<lattice.sizeOfNodePerFrame(0);i++){
    std::cout << "nodes[0][" << i << "]=" << nodes[i].word << std::endl;
  }
  lattice.dump();
  
}

int main(){
  //test1();
  test2();
  return 0;
}
