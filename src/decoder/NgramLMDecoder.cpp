#include "decoder/NgramLMDecoder.h"

Result NgramLMDecoder::decode(const std::string& str){
  Result result;
  Lattice &word_lattice = generate_lattice(_dic,str);
  //_word_lattice.dump();
  Lattice &hyp_lattice = forward(word_lattice);
  result = backtrace(hyp_lattice);
  return result;
}

NBestResult NgramLMDecoder::getNBestHyp(int num_nbest){

  NBestResult nbestResult;
  int n_best = std::min(num_nbest,(int)_hyp_lattice.sizeOfNodePerFrame(_hyp_lattice.sizeOfFrame()-1));
  for(int i=0;i<n_best;i++){
    Result result = backtrace(_hyp_lattice, i);
    nbestResult.push_back(result);
  }
  return nbestResult;
}

double NgramLMDecoder::get_lm_score(const Node *node, int ngram_size){
  std::vector<std::string> words;
  std::stringstream s;
  Node *t_node=(Node*)node;
  for(int i=0;i<ngram_size;i++){
    if(is_debug()){
      LOG(DEBUG) << i << "-gram:" << t_node->word;
      LOG(DEBUG) << "pointer:" << t_node;
    }
    if(t_node->is_bos()){
      s << "<s>";
    }
    else if(t_node->is_eos()){
      s << "</s>";
    }
    else{
      s << t_node->word << _delimiter << t_node->read;
    }
    words.push_back(s.str());
    s.str("");
    if(t_node->prev == (Node*)NULL) break;
    t_node = t_node->prev;
    
  }
  std::reverse(words.begin(),words.end());
  if(is_debug()){
    LOG(DEBUG) << join(" ",words) << "\t" << _slm.getProb(words,words.size());
  };
  return _slm.getProb(words,words.size());
}

double qsort_compare(const void* a, const void* b){
  return ((Node*)b)->score - ((Node*)a)->score;
}

bool operator<(const Node& a, const Node& b){
  return a.score < b.score;
}

bool operator>(const Node& a, const Node& b){
  return a.score > b.score;
}

Lattice& NgramLMDecoder::forward(const Lattice &word_lattice){
  //LOG(DEBUG) << "graph size=" << word_lattice.sizeOfFrame() << std::endl;
  
  int ngram_size = _slm.getNgramLength();
  for(size_t i=0;i<word_lattice.sizeOfFrame();i++){ // loop frame
    std::vector<Node> current_frame_hyp_list;
    _hyp_lattice.addFrame(i); // dynamic memory allocate i-th Frame
    for(size_t j=0;j<word_lattice.sizeOfNodePerFrame(i);j++){ // extract current node
      Node *node = &word_lattice[i][j];
      if(is_debug()){ LOG(DEBUG) << i << "graph word=" << node->word << _delimiter << node->read;}
      if(node->is_bos()){
	Node hyp_node(node->word, node->read, node->endpos); 
	hyp_node.score = 0.0;
	current_frame_hyp_list.push_back(hyp_node);
	// <s> uni-gram is not calculated because of large log probability .
      }else{
	int prev_pos = node->get_prev_pos(); // trace prev history
	if(is_debug()){ LOG(DEBUG) << "prev pos=" << prev_pos; }
	  
	vector<Node>::iterator prev_node;
	for(size_t k=0;k<_hyp_lattice.sizeOfNodePerFrame(prev_pos);k++){
	  Node hyp_node(node->word, node->read, node->endpos);
	  hyp_node.score = 0.0;
	  Node *prev_node = &_hyp_lattice[prev_pos][k];
	  if(is_debug()){
	    LOG(DEBUG) << "prev pointer=" << prev_node;      
	    LOG(DEBUG) << i << "prev word=" << prev_node->word << _delimiter << prev_node->read;
	  }
	  hyp_node.prev = (Node*)prev_node;
	  double lmscore = get_lm_score(&hyp_node, ngram_size);
	  hyp_node.score = lmscore + prev_node->score;
	  if(is_debug()){LOG(DEBUG) << i << ":lm score=" << hyp_node.score ;}
	  current_frame_hyp_list.push_back(hyp_node);
	}
      }
    }
    //--- beam pruning in i-th frame---
    //std::qsort(current_frame_nodes,current_frame_nodes.size(),sizeof(Node),qsort_compare);
    std::sort(current_frame_hyp_list.begin(),current_frame_hyp_list.end(),std::greater<Node>());

    // beam prune
    if(current_frame_hyp_list.size()>_beam_size){
      current_frame_hyp_list.erase(current_frame_hyp_list.begin()+_beam_size, current_frame_hyp_list.end());
    }

    // push current hyp list
    if(is_debug()){
      LOG(DEBUG) << i << "-th frame add hyp list:" << current_frame_hyp_list.size() ;
    }
    _hyp_lattice.expandNode(i, current_frame_hyp_list);
  }
  return _hyp_lattice;
}

Result NgramLMDecoder::backtrace(const Lattice &hyp_lattice, int n_best){
  Result result;
  std::vector<std::string> surfaceList;
  std::vector<std::string> readingList;

  Node *node = hyp_lattice[hyp_lattice.sizeOfFrame()-1][n_best].prev; // eos.prev
  double score = hyp_lattice[hyp_lattice.sizeOfFrame()-1][n_best].score; // eos.score
  // traverse
  while(!node->is_bos()){
    if(is_debug()){ LOG(DEBUG) << node->word << ":" << node->read << ", "<< node->score;}
    surfaceList.push_back(node->word);
    readingList.push_back(node->read);
    node = node->prev;
  }
  reverse( surfaceList.begin(), surfaceList.end() );
  reverse( readingList.begin(), readingList.end() );
  result.surfaceList = surfaceList;
  result.readingList = readingList;
  result.score = score;
  return result;
}
