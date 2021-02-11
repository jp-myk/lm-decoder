#include "decoder/Decoder.h"
Decoder::Decoder()
  :_delimiter(":")
  ,_loadDicFromFile(false)
  ,_loadModelFromFile(false)
  ,_dic(nullptr)
  ,_model(nullptr){}

Decoder::~Decoder(){
  if(_loadModelFromFile && _model!=nullptr) {
    delete _model;
    _model = nullptr;    
  }
  if(_loadDicFromFile &&_dic!=nullptr) {
    delete _dic;
    _dic   = nullptr;
  }
};

int Decoder::setDic(const char* dicfile){
  if (_dic != nullptr) return 1;
  _dic = new Dic();
  _dic->read(dicfile);
  _loadDicFromFile = true;
  return 0;
}

int Decoder::setModel(const char* modelfile){
  if (_model != nullptr) return 1;
  _model = new Model();
  _model->read(modelfile);
  _loadModelFromFile = true;
  return 0;
}
int Decoder::setDic(Dic* dic){
  if (_dic != nullptr) return 1;
  _dic = dic;
  return 0;
}
int Decoder::setModel(Model* model){
  if (_model != nullptr) return 1;
  _model = model;
  return 0;
}

Lattice& Decoder::generate_lattice(Dic* dic, const std::string& str){
  std::vector<std::string> char_ary = split("",str);
  int len = static_cast<int>(char_ary.size());
  int frame_size = len+2; // 2 means <sos> and <eos>
  _word_lattice.resize(frame_size);
  for(int i=0;i<frame_size;i++){
    std::vector<Node> nodes;
    std::vector<std::string> words,reads;
    std::vector<std::string>::iterator it, it_r; // words,reads's iterator
    std::unordered_set<std::string> word_set,read_set;
    
    if(i==0){
      Node bos("<s>", "", 1); // change for SLM
      _word_lattice.addNode(i, bos);
    }else if(i==frame_size-1){
      Node eos("</s>", "", len+2);
      _word_lattice.addNode(i, eos);
    }else{
      words = dic->lookupDic(substr_utf8(str, i-1)); // prefix common search to look up Dic

      // detect UNK word
      if (words.size() == 0) {
	Node node(char_ary[i-1], "UNK", i+1); // word->read node
	_word_lattice.addNode(i,node);
	continue;
      }
      
      for(it=words.begin();it!=words.end();it++){
	std::string word = *it;
	if(word_set.count(word)>0) continue;
	if(word == "") continue; // sometimes extract empty word
	word_set.insert(word);
	int wlen = strlen_utf8(word);
	reads=dic->getRead(word);
	for(it_r=reads.begin();it_r!=reads.end();it_r++){
	  std::string read = *it_r;
	  if(read_set.count(read)>0) continue;
	  read_set.insert(read);
 	  Node node(word, read, i+wlen); // word->read node
	  _word_lattice.addNode(i+wlen-1,node);
	}
	read_set.clear();
      }
      word_set.clear();
    }
  }
  return _word_lattice;
}

double Decoder::get_node_score(const Node& node){
  double score=0.0;
  std::vector<std::string> feat_vec = _feature.extract_node_feature(node);
  score =  _model->get_score(feat_vec);
  //printf("%s:%s:%d:%s, score=%f\n",__FILE__,__FUNCTION__,__LINE__,node.word.c_str(), score);
  return score;
}

double Decoder::get_edge_score(const Node& prev_node, const Node& node){
  double score=0.0;
  if (prev_node.empty()) return 0.0;
  std::vector<std::string> feat_vec = _feature.extract_edge_feature(prev_node,node);
  score =  _model->get_score(feat_vec);
  //printf("%s:%s:%d:%s %s, score=%f\n",__FILE__,__FUNCTION__,__LINE__,prev_node.word.c_str(),node.word.c_str(), score);
  return score;
}



Lattice& Decoder::viterbi(Lattice &word_lattice){
  std::vector<std::pair<std::string, std::string> > result;
  for(size_t i=0;i<word_lattice.sizeOfFrame();i++){ // loop frame
    //_hyp_lattice.addFrame(i); // dynamic memory allocate i-th Frame
    for(size_t j=0;j<word_lattice.sizeOfNodePerFrame(i);j++){ // loop i-th frame's node
      Node *node = &word_lattice[i][j];
      if(is_debug()){ LOG(DEBUG) << i << "graph word=" << node->word << _delimiter << node->read;}
      if(node->is_bos()) continue;
      node->score = -999.999; // init forward score
      double score_cache = get_node_score(*node);
      int prev_pos = node->get_prev_pos();

      //vector<Node>::iterator prev_node;
      for(size_t k=0;k<word_lattice.sizeOfNodePerFrame(prev_pos);k++){ // loop prev frame's node
	Node *prev_node = &word_lattice[prev_pos][k];
	// 一つ前の生起コスト+接続コスト+今の生起コスト
	double tmp_score = (*prev_node).score+get_edge_score(*prev_node, *node) + score_cache;
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

Result Decoder::backtrace(const Lattice &word_lattice, int n_best){
  Result result;
  std::vector<std::string> surfaceList;
  std::vector<std::string> readingList;

  const Node *node   = word_lattice[word_lattice.sizeOfFrame()-1][0].prev; // eos.prev
  double score =  word_lattice[word_lattice.sizeOfFrame()-1][0].score; // eos.score
  // traverse
  while(node->is_bos() == false){
    if(is_debug()){ LOG(DEBUG) << node->word << ":" << node->read << ", "<< node->score;}
    surfaceList.emplace_back(node->word);
    readingList.emplace_back(node->read);
    node = node->prev;
  }
  reverse( surfaceList.begin(), surfaceList.end() );
  reverse( readingList.begin(), readingList.end() );
  result.surfaceList = surfaceList;
  result.readingList = readingList;
  result.score = score;
  return result;
}

bool operator>(const Arc& a, const Arc& b){
  return a.f > b.f;
}

bool operator<(const Arc& a, const Arc& b){
  return a.f < b.f;
}

Result Decoder::decode(const std::string& input){
  Result result;
  Lattice& word_lattice = generate_lattice(_dic,input);
  Lattice& hyp_lattice = viterbi(word_lattice);
  //_word_lattice.dump();
  result = backtrace(hyp_lattice);
  return result;
}

NBestResult Decoder::getNBestHyp(int num_nbest){
  NBestResult nbestResult;
  nbestResult= backward_a_star(_word_lattice,num_nbest);
  int n_best = std::min(num_nbest,(int)nbestResult.size());
  return nbestResult;
}


// long fx;  // f(x) = g(x) + h(x): cost function for A* search
NBestResult Decoder::backward_a_star(Lattice& word_lattice, int n_best){
  NBestResult nbestResult;
  //std::priority_queue<Path, std::vector<Path>, std::greater<Path> > que;
  std::priority_queue<Arc, std::vector<Arc> > que;
  std::unordered_map<int, Arc> arcMap;
  std::unordered_map<int, Node> stateMap;

  int state_id = 0;
  int nbest_no = 0;
  Arc arc;
  Node node = word_lattice[word_lattice.sizeOfFrame()-1][0]; // eos.node
  {
    arc.src_id = state_id;
    arc.dest_id = -1;
    arc.g = node.score;
    arc.f = 0.0;
    arc.h = 0.0;
    que.push(arc);
    stateMap[state_id] = node;
    arcMap[state_id] = arc;
    state_id++;
  }
  
  que.push(arc);

  while(!que.empty()){
    arc = que.top(); que.pop();
    node = stateMap[arc.src_id];
    // notice to return same pointer address
    if(is_debug()){
      LOG(DEBUG) << "path pointer=" <<  &node << std::endl;
    }

    if(is_debug()){
      std::cout << "[DEBUG] top=" << node.word << _delimiter << node.read << std::endl;
    }
    //if(path.next_id != -1){
    //  if(is_debug()) std::cout << "[DEBUG] top next=" << state_map[path.next_id].node.word << std::endl;
    //}
    if(node.is_bos()){
      //
      // extract N-best !!!
      //
      Result result;
      std::vector<std::string> surfaceList;
      std::vector<std::string> readingList;
      Arc t_arc = arc;
      //std::vector<std::string> nbest_tokens;
      // </s>
      //std::string token = node.word + _delimiter + node.read ;
      //surfaceList.emplace_back(node.word);
      //readingList.emplace_back(node.read);
      
      while(t_arc.dest_id != -1){
	t_arc = arcMap[t_arc.dest_id];
	if (t_arc.dest_id == -1) break;
	Node t_node = stateMap[t_arc.src_id];
	surfaceList.emplace_back(t_node.word);
	readingList.emplace_back(t_node.read);
	if(is_debug()){
	  std::cout << t_node.word << ":" << t_node.read << ":(id=" << t_arc.src_id << ") " << std::endl;
	}
      }
      
      if(is_debug()){
	//LOG(DEBUG) << nbest_no+1 << "-BEST\t" << join(" ",nbest_tokens) << "\t" << arc.f;
      }
      result.surfaceList = surfaceList;
      result.readingList = readingList;
      result.score = arc.f;
      nbestResult.emplace_back(result);
      nbest_no++;
      if(nbest_no == n_best){
	break;
      }
	
    }else{
      int prev_pos = node.get_prev_pos();
      std::vector<Node>::iterator prev_node;
      for(size_t i=0;i<_word_lattice.sizeOfNodePerFrame(prev_pos);i++){
	Node prev_node = _word_lattice[prev_pos][i];
	double edge_score = get_edge_score(prev_node, node);
	double h_score =  arc.h + edge_score + get_node_score(node); // g = g + h(edge + node)
	
	Arc new_arc;
	new_arc.src_id = state_id;
	new_arc.dest_id = arc.src_id;
	new_arc.h = h_score;
	new_arc.g = prev_node.score ;
	new_arc.f = new_arc.g + new_arc.h;
	stateMap[state_id] = prev_node;
	arcMap[state_id] = new_arc;
	state_id++;
	
	que.push(new_arc);
	if(is_debug()){
	  LOG(DEBUG) << prev_node.word << "->" << node.word 
		     << "score=" << new_arc.f << std::endl;
	}
	
      }
    }
  }
  
  return nbestResult;
}

