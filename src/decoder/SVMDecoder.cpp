#include "decoder/SVMDecoder.h"

SVMDecoder::SVMDecoder()
  :_delimiter(":")
  ,_loadDicFromFile(false)
  ,_loadModelFromFile(false)
  ,_dic(nullptr)
  ,_model(nullptr)
  ,_penalty(0.05){}

SVMDecoder::~SVMDecoder() {
  if(_loadModelFromFile && _model!=nullptr) {
    delete _model;
    _model = nullptr;    
  }
  if(_loadDicFromFile &&_dic!=nullptr) {
    delete _dic;
    _dic   = nullptr;
  }
}

int SVMDecoder::setDic(const char* dicfile){
  if (_dic != nullptr) return 1;
  _dic = new Dic();
  _dic->read(dicfile);
  _loadDicFromFile = true;
  return 0;
}

int SVMDecoder::setModel(const char* modelfile){
  if (_model != nullptr) return 1;
  _model = new SVM();
  _model->read(modelfile);
  _loadModelFromFile = true;
  return 0;
}

int SVMDecoder::setDic(Dic* dic){
  if (_dic != nullptr) return 1;
  _dic = dic;
  return 0;
}

int SVMDecoder::setModel(SVM* model){
  if (_model != nullptr) return 1;
  _model =model;
  return 0;
}

bool SVMDecoder::_is_correct_node(Node& node, std::unordered_map<int,Node>& gold){
  Node n=gold[node.endpos];
  if(node.word == n.word){
    return true;
  }else{
    return false;
  }
}
bool SVMDecoder::_is_correct_edge(Node& prev_node, Node& node, std::unordered_map<int, Node>& gold){
  if(_is_correct_node(node, gold) && _is_correct_node(prev_node, gold)){
    return true;
  }else{
    return false;
  }
}

double SVMDecoder::get_node_score(Node& node, std::unordered_map<int, Node>& gold){
  double score=0.0;
  std::string feat;

  if(_is_correct_node(node, gold)){
    score -= _penalty;
  }
  std::vector<std::string> feat_vec = _feature.extract_node_feature(node);
  score =  _model->get_score(feat_vec);
  //printf("%s:%s:%d:score=%f\n",__FILE__,__FUNCTION__,__LINE__,score);
  return score;
}

double SVMDecoder::get_edge_score(Node& prev_node, Node& node, std::unordered_map<int, Node>& gold){
  double score=0.0;
  if (prev_node.empty()) return 0.0;
  if(_is_correct_edge(prev_node, node, gold)){
    score -= _penalty;
  }
  std::vector<std::string> feat_vec = _feature.extract_edge_feature(prev_node,node);
  score =  _model->get_score(feat_vec);
  //printf("%s:%s:%d:score=%f\n",__FILE__,__FUNCTION__,__LINE__,score);
  return score;
}



Lattice& SVMDecoder::viterbi_with_penalty(Lattice &word_lattice, std::unordered_map<int,Node>& gold){
  std::vector<std::pair<std::string, std::string> > result;
  for(size_t i=0;i<word_lattice.sizeOfFrame();i++){ // loop frame
    //std::vector<Node>::iterator node;
    //_hyp_lattice.addFrame(i); // dynamic memory allocate i-th Frame
    for(size_t j=0;j<word_lattice.sizeOfNodePerFrame(i);j++){ // loop i-th frame's node
      Node *node = &word_lattice[i][j];
      if(is_debug()){ LOG(DEBUG) << i << "graph word=" << node->word << _delimiter << node->read;}
      if(node->is_bos()) continue;
      node->score = -999.999; // init forward score
      double score_cache = get_node_score(*node, gold);
      int prev_pos = node->get_prev_pos();

      //vector<Node>::iterator prev_node;
      for(size_t k=0;k<word_lattice.sizeOfNodePerFrame(prev_pos);k++){ // loop prev frame's node
	Node *prev_node = &word_lattice[prev_pos][k];
	// 一つ前の生起コスト+接続コスト+今の生起コスト
	double tmp_score = (*prev_node).score+get_edge_score(*prev_node, *node, gold) + score_cache;
	if(is_debug()){
	  printf("[debug]%s:%s:%d\tprev=%s, token=%s:%s, prev score=%f >? curr score=%f\n",
		 __FILE__,__FUNCTION__,__LINE__,
		 prev_node->word.c_str(),node->word.c_str(),node->read.c_str(),tmp_score,node->score);
	}
	// store max prev node
	if(tmp_score>=node->score){
	  if(is_debug()){
	    printf("[debug]%s:%s:%d\tupdate best path score\tprev=%s, token=%s:%s, score=%f\n",
		   __FILE__,__FUNCTION__,__LINE__,
		   prev_node->word.c_str(),node->word.c_str(),node->read.c_str(),tmp_score);
	  }
	  node->score = tmp_score;
	  node->prev = (Node*)&(*prev_node); // update best prev path
	}
      }
    }
  }
  return word_lattice;
}
