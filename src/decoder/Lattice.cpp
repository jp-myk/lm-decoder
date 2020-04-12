#include "decoder/Lattice.h"

Lattice::Lattice():
  _graph(nullptr),
  _node_size(nullptr),
  _frame_size(0)
{}

Lattice::Lattice(size_t frame_size):
  _graph(nullptr),
  _node_size(nullptr),
  _frame_size(0){
  resize(frame_size);
}

Lattice::~Lattice(){
  clear();
}

Node* Lattice::operator[](int i){
  return (Node*)_graph[i];
}

Node* const Lattice::operator[](int i) const {
  return (Node*)_graph[i];
}

Lattice& Lattice::operator=(const Lattice& o){
    _graph = o._graph;
    _frame_size = o._frame_size;
    _node_size = o._node_size;
    return *this;
}

void Lattice::addFrame(int n_frame){
  //std::cout << "addFrame::graph size=" << sizeof(_graph) << std::endl;
  _graph = (Node**)realloc(_graph, (n_frame+1)*sizeof(Node*));
  _node_size = (size_t*)realloc(_node_size, (n_frame+1)*sizeof(size_t));
  if(! _graph || ! _node_size){
    std::cout << "fail." << std::endl;
    exit(1);
  }
  _graph[n_frame] = (Node*)NULL;
  _node_size[n_frame] = 0;
  _frame_size=n_frame+1;
}

void Lattice::expandNode(int n_frame, const std::vector<Node>& nodes){
  _graph[n_frame] = new Node[nodes.size()];
  _node_size[n_frame] = nodes.size();
  for(size_t l=0;l<nodes.size();l++){
    _graph[n_frame][l] = nodes[l];
  }
  //std::cout << "test" << n_frame <<_frame_size << _graph[0][0].read << std::endl;
}


void Lattice::addNode(size_t n_frame, const Node& node){
  _node_size[n_frame] +=1;
  size_t node_size = _node_size[n_frame];
  
  // tmp alloc
  Node *t_node = new Node[node_size];
  for(size_t i=0;i<node_size-1;i++){
    t_node[i] = _graph[n_frame][i]; 
  }
  t_node[node_size-1] = node;
  
  // dirty tmp delete
  if(_graph[n_frame] != (Node*)NULL){
    //LOG(DEBUG) << "delete node";
    delete [] _graph[n_frame];
  }
  
  // new alloc
  _graph[n_frame] = new Node[node_size];
  for(size_t i=0;i<node_size;i++){
    _graph[n_frame][i] = t_node[i];
  }
  
  delete [] t_node;
}  
  


void Lattice::resize(size_t frame_size){
  clear();
  _frame_size=frame_size;
  _graph = (Node**)malloc(_frame_size*sizeof(Node*));
  _node_size = (size_t*)malloc(_frame_size*sizeof(size_t));
  for(size_t i=0;i<_frame_size;i++){
    _graph[i] = (Node*)NULL;
    _node_size[i] = 0;
  }
}


void Lattice::clear(){
  //LOG(DEBUG) << "frame_size=" << _frame_size;
  for(size_t i=0;i<_frame_size;i++){
    if(_graph[i]!=(Node*)NULL){
      //LOG(DEBUG) << "delete nodes " << i;
      delete [] _graph[i];
    }
    //LOG(DEBUG) << "delete graph";	
    _graph[i]=(Node*)NULL;
  }
  if(_graph!=(Node**)NULL) free(_graph);
  if(_node_size!=(size_t*)NULL) free(_node_size);
  _graph = (Node**)NULL;
  _node_size = (size_t*)NULL;
  _frame_size = 0;
}


void Lattice::dump(){
  for(size_t i=0;i<_frame_size;i++){
    std::cout << "[" << i << "] ";
    //Node *nodes = (Node*)_graph[i];
    size_t node_size = _node_size[i];
    std::string prev_word="not exist";
    int prev_pos = -1;
    //for(it2=nodes.begin();it2!=nodes.end();it2++){
    for(size_t j=0;j<node_size;j++){
      Node node = _graph[i][j];
      if(node.prev != NULL){
	prev_word = node.prev->word;
	prev_pos =  node.prev->endpos-1;
      }
      std::cout << node.word << ":" << node.read << "(score=" << node.score << ",endpos=" << node.endpos 
	   << ",prev=" << prev_word << ",prev_pos=" << prev_pos << ") ";
      if(i == (size_t)prev_pos){
	std::cout << std::endl;
	std::cout << "[CRIT] this lattice has self-loop!!" << std::endl;
	//exit(1);
      }

    }
    cout << endl;
  }
  //return 0;
}

int Lattice::get_prev_pos(Node& node){
  if(node.is_eos()){
    int startpos = node.endpos - 2;
    return startpos;
    
  }else if(node.is_bos()){
    return -1;
    
  }else{
    int startpos=node.endpos-node.length()-1;
    return startpos;
  }
}

size_t Lattice::sizeOfFrame()const{
  return _frame_size;
}

size_t Lattice::sizeOfNodePerFrame(size_t n_frame)const{
  return _node_size[n_frame];
}
