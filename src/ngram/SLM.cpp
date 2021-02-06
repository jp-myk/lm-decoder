#include "ngram/SLM.h"  

int NgramNodeCompare(const void *n1, const void *n2){
  //  printf("%d - %d\n", ((NgramNode*)n1)->id, ((NgramNode*)n2)->id);
  return ((NgramNode*)n1)->id - ((NgramNode*)n2)->id;
}

SLM::SLM():
  _debug(false)
{}

SLM::~SLM(){
  int i;
  for(i=0;i<_ngram_len;i++){
     delete[] _nodes[i];
  }
  delete[] _nodes;
}

/*
// ngrams in ARPA format LM must be already sorted. 
// ngrams are already inputed to _nodes
// 1: input word array
// 2: ngram node vectors (1-gram=node[0], 2gram=node[1], 3gram=node[2]...)
// 3: next ngram element size
// 4: current ngram level
// 5: target level for adding ngram
// 6: pos number to input ngram vector
*/
int SLM::_putNgramNodeData(std::vector<std::string> &word_array, NgramNode *nd_vec, int nelem, int curr_level, int target_level, int pos){
  NgramNode ref, *nd;
  //nd = (NgramNode*)malloc(sizeof(NgramNode*));
  
  // ngram nodeがngram node vectorにあるか検索(start posからの配列とelement数が必要)
  // 既にinsertしている
  ref.id = _word2id[word_array[curr_level-1]];
  if(_debug) printf("add.id=%d, nelem=%d, curr_level=%d, target_level=%d, pos=%d\n", ref.id, nelem, curr_level, target_level, pos);
  
  // target id, ngram vector from offset start, # of offset to end
  nd = (NgramNode*)bsearch(&ref, nd_vec, nelem, sizeof(NgramNode), NgramNodeCompare);
    
  if(nd == NULL){
    qsort(nd_vec, nelem, sizeof(NgramNode), NgramNodeCompare);
    nd = (NgramNode*)bsearch(&ref, nd_vec, nelem, sizeof(NgramNode), NgramNodeCompare);
    if(nd == NULL){
      printf("ARPA format is broken. ngrams are sorted ?\n");
      for(int i=0; i<nelem;i++){
	printf("level=%d, ngram vec id=%d\n", curr_level, nd_vec[i].id);
      }
      exit(0);
      return -1;
    }
  }
  
  if(curr_level == target_level-1){
    // register new node
    if(nd->nextpos < 0){
      nd->nextpos = pos;
      if(_debug) printf("register new node:nextpos=%d\n",pos);
    }else{
      if(_debug) printf("exist node:nextpos=%d\n",pos);
    }
    nd->nelem++;
    if(_debug) printf("start next pos=%d, offset=%d\n",pos, nelem);
    
  }else{
    // 再帰的に次のngram levelに移動
    _putNgramNodeData(word_array, &_nodes[curr_level][nd->nextpos], nd->nelem,
			   curr_level+1, target_level, pos);
  }
  return 0;
}


void SLM::_updateNgramNodeData(std::vector<std::string>& word_array, NgramNode *nn2){
  NgramNode *nn1;
  
  nn1 = _search_node(word_array, _nodes[0], _ngram_size[0], 1, (int)word_array.size()); // wordarray, 追加node, first_ngram,  target_ngrams
  
  if(nn1 != NULL){
    if(_debug) std::cout << "update node" << std::endl;
    // ngram hit
    nn1->prob += ProbtoLogP( LogPtoProb(nn1->prob)+LogPtoProb(nn2->prob) );
    nn1->alpha += ProbtoLogP( LogPtoProb(nn1->alpha)+LogPtoProb(nn2->alpha) );
    
  }else{
    printf("ERROR:updateNgramNodeData, not exist %s\n", join(" ", word_array).c_str());
    exit(0);
    //return NULL;
  }
  //return prob;
}


NgramNode* SLM::_search_node(std::vector<std::string> &word_array, NgramNode *nd_vec, 
			  int nelem, int curr_level, int target_level){
  NgramNode ref, *nd;
  ref.id = _word2id[word_array[curr_level-1]];

  if(curr_level == _ngram_len){
    return (NgramNode*)bsearch(&ref, nd_vec, nelem, sizeof(NgramNode), NgramNodeCompare);
  }else{
    nd = (NgramNode*)bsearch(&ref, nd_vec, nelem, sizeof(NgramNode), NgramNodeCompare);

    if(curr_level == target_level || nd == NULL){
      return nd;
    }else{
      return _search_node(word_array, &_nodes[curr_level][nd->nextpos], nd->nelem,
			       curr_level+1, target_level);
    }
  }
}

double SLM::_getProb0(std::vector<std::string>& word_array, int len){
  NgramNode *nn1, *nn2;
  double prob = 0.0;
  nn1 = _search_node(word_array, _nodes[0], _ngram_size[0], 1, len); // wordarray, 追加node, first_ngram,  target_ngrams
  
  if(nn1 != NULL){
    // ngram hit
    prob = nn1->prob;
    return prob;
  }else{
    if(len == 1){
      // if even not hit 1-gram, use 0-gram uniform probability. no use <unk> probability
      prob = 1.0/(int)_word2id.size();
      //prob = 0;
    }else{
      nn2 = _search_node(word_array, _nodes[0], _ngram_size[0], 1, len-1); // wordarray, 追加node, first_ngram,  target_ngrams 
      if(nn2 != NULL){
	// backoff prob = alpha(y|x) * prob(z|y)
	// backoff logprob = logalpha(y|x) + logprob(z|y)
	std::vector<std::string> lower;
	for(size_t i=1;i<word_array.size();i++)
	  lower.push_back(word_array[(int)i]);
	
	prob = nn2->alpha + _getProb0(lower, len-1);
      }else{
	prob = 0;
	std::vector<std::string> lower;
	for(size_t i=1;i<word_array.size();i++)
	  lower.push_back(word_array[(int)i]);
	prob = _getProb0(lower, len-1);
      }
    }

  }
  return prob;
}

/*
 * @return the maximal N-gram order to be used
 */
int SLM::getNgramLength(){
  return _ngram_len;
}


/*
 * @return the size of N-gram 
 */
int SLM::getNgramSize(int n_order){
  return _ngram_size[n_order];
}



//-------------------------------
// return Ngram Probability from Sring Words
//-------------------------------
double SLM::getProb(std::vector<std::string> &word_array, int len){
  double retval = 0.0;
  int i,j;
  int ngram_len = (int)word_array.size()<len ? (int)word_array.size() : len;
  for(i=0;i<(int)word_array.size();i++){
    if(i+ngram_len > (int)word_array.size()) break;
    std::vector<std::string> buf;
    for(j=0;j<ngram_len;j++){
      buf.push_back(word_array[i+j]);
    }
    // std::cout << "debug" << join(" ", buf) << std::endl;
    retval += _getProb0(buf, ngram_len);
    // std::cout << "debug" << toString(retval) << std::endl;
  }
  return retval;
}

//-------------------------------
// return Ngram Probability from ID Words
//-------------------------------
double SLM::getProb(std::vector<int> &word_array, int len){
  double retval = 0.0;
  int i,j;
  int ngram_len = (int)word_array.size()<len ? (int)word_array.size() : len;
  for(i=0;i<(int)word_array.size();i++){
    if(i+ngram_len > (int)word_array.size()) break;
    std::vector<std::string> buf;
    for(j=0;j<ngram_len;j++){
      buf.push_back(_getWordStr(word_array[i+j]));
    }
    // std::cout << "debug" << join(" ", buf) << std::endl;
    retval += _getProb0(buf, ngram_len);
    // std::cout << "debug" << toString(retval) << std::endl;
  }
  return retval;
}

//-------------------------------
// return Ngram Probability from String Sentence
//-------------------------------
double SLM::getProb(std::string& str, int len){
  double retval = 0.0;
  int i,j;
  std::vector<std::string> word_array = split(" ", str);
  int ngram_len = (int)word_array.size()<len ? (int)word_array.size() : len;
  for(i=0;i<(int)word_array.size();i++){
    if(i+ngram_len > (int)word_array.size()) break;
    std::vector<std::string> buf;
    for(j=0;j<ngram_len;j++){
      buf.push_back(word_array[i+j]);
    }
    retval += _getProb0(buf, ngram_len);
  }
  return retval;
}

double SLM::getEntropy(std::vector<std::string>& word_array, int order){
  double ll = getProb(word_array, order);
  return -1*ll/(int)word_array.size()/log(2);
}

double SLM::getPPL(std::vector<std::string>& word_array, int order){
  //double ll = getProb(word_array, order);
  return std::pow(2, getEntropy(word_array, order));
}


bool SLM::isNode(std::vector<std::string> &word_array){
  NgramNode *nd = _search_node(word_array, _nodes[0], _ngram_size[0], 1, (int)word_array.size()); // wordarray, 追加node, first_ngram,  target_ngrams
  if(_debug && nd !=NULL) printf("id:%d, prob:%f\n", nd->id, nd->prob);
  return nd != NULL ? true : false;
}



std::vector<std::string> SLM::_dumpNgram0(int curr_level, int target_level, int pos, int nelem, std::string& buf){
  NgramNode *nd,*nd2;
  std::vector<std::string> retval;
  if (_debug) printf("curr_level=%d, target_level=%d, pos=%d, nelem=%d, prev=%s\n", curr_level, target_level, pos, nelem, buf.c_str());
  nd = &_nodes[curr_level-1][pos];
  
  if(nd != NULL){
    if(curr_level == target_level){
      for(int i=pos;i<pos+nelem;i++){
	nd2 = &_nodes[curr_level-1][i];
	std::string buf2 = (curr_level == 1) ? _id2word[nd2->id] : buf+" "+_id2word[nd2->id];
	if(_debug) printf("pos=%d, curr=%d, word=%s\n", i, curr_level, buf2.c_str());
	retval.push_back(buf2);
      }
      return retval;
    }else{      
      for(int i=pos;i<pos+nelem;i++){
	nd2 = &_nodes[curr_level-1][i];
	std::string buf2 = (curr_level == 1) ? _id2word[nd2->id] : buf+" "+_id2word[nd2->id];
	std::vector<std::string> buf_vec1, buf_vec2;
	if(_debug) printf("curr=%d, word=%s\n", curr_level, buf2.c_str());
	// vector merge
	buf_vec1 = _dumpNgram0(curr_level+1, target_level, nd2->nextpos, nd2->nelem, buf2);	
	set_union(retval.begin(), retval.end(), buf_vec1.begin(), buf_vec1.end(), back_inserter(buf_vec2));
	retval = buf_vec2;

      }

    }
    
  }
  if(_debug) std::cout << "dumpngram error" << std::endl;
  return retval;
}


//-------------------------------
// return Dumping Ngram Vector
//-------------------------------
std::vector<std::string> SLM::dumpNgram(int len){
  std::string buf = "";
  std::vector<std::string> ret;  

  if(_debug) printf("\\%d-gram:\n", len);
  ret = _dumpNgram0(1,len,0, _ngram_size[0], buf);
  if(_debug) printf("n=%d\n", (int)ret.size());
  /*
  std::vector<std::string>::iterator it;
  for(it=ret.begin();it!=ret.end();it++){
  if(_debug) std::cout << *it << std::endl;
  }
  */
  return ret;
}

//-------------------------------
// get Vocab
//-------------------------------
std::vector<std::string> SLM::getVocab(){
  std::vector<std::string> retval;  
  
  WORD2ID::iterator it;
  for(it=_word2id.begin();it!=_word2id.end();it++){
    retval.push_back(it->first);
  }
  
  return retval;
}

int SLM::getWordID(std::string& word){
  int retval = _word2id[word];
  return retval != 0 ? retval : -1;
}
std::string SLM::_getWordStr(int wordID){
  std::string retval =_id2word[wordID];
  return retval != "" ? retval : "<UNK>";
}

//---------------------------------------
// read ARPA Language Model from file
//---------------------------------------
int SLM::readLM(const char *filename){
  std::ifstream ifs;
  char buf[1024];
  //int ngram_size[10]; // MAX_NGRAM=10
  int n,m;
  _ngram_len = 0;
  double prob, alpha;
  ifs.open(filename);
  if(_debug) std::cout << "/* read header */" << std::endl;
  while(ifs && ifs.getline(buf, sizeof(buf))){
    if (strncmp(buf,"\\data\\",6) == 0){
      //break;
      continue;
    }
    if (strncmp(buf,"ngram ",6) == 0) {
      sscanf(buf+6,"%d=%d",&n,&m);
      if (n < 1 || n > MAX_NGRAM) {
	fprintf(stderr,"SLMReadLM ERROR: Can't handle this LM's n-gram length; limit = %d\n",
		MAX_NGRAM);
	return 1; // FAIL
      }
      _ngram_size[n-1] = m;
      if (n > _ngram_len)
	_ngram_len = n;
    }else if (buf[0] == '\\'){
      break;
    }
  }
  
  /* memory allocate */
  //_nodes = (NgramNode**)malloc(sizeof(NgramNode)*_ngram_len+1);
  //memset(_nodes, 0, sizeof(NgramNode)*_ngram_len+1);
  //for(n=0;n<_ngram_len;n++){
  //  _nodes[n] = (NgramNode*)malloc(sizeof(NgramNode)*_ngram_size[n]+1);
  //  memset(_nodes[n], 0, sizeof(NgramNode)*_ngram_size[n]+1);
  //  if(_debug) printf("%d-ngram=%d\n", n+1, _ngram_size[n]);
  //}

  //_nodes = (NgramNode**)malloc(sizeof(NgramNode*)*_ngram_len);
  //memset(_nodes, 0, sizeof(NgramNode*)*_ngram_len);
  //for(n=0;n<_ngram_len;n++){
  //  _nodes[n] = (NgramNode*)malloc(sizeof(NgramNode)*_ngram_size[n]);
  //  memset(_nodes[n], 0, sizeof(NgramNode)*_ngram_size[n]);
  //  if(_debug) printf("%d-ngram=%d\n", n+1, _ngram_size[n]);
  //}

  _nodes = new NgramNode*[_ngram_len];
  for(n=0;n<_ngram_len;n++){
    _nodes[n] = new NgramNode[_ngram_size[n]];
    if(_debug) printf("%d-ngram=%d\n", n+1, _ngram_size[n]);
  }
  

  /* Ngram start */
  if(_debug) std::cout << "/* ngram start */" << std::endl;
  n = 0;
  int pos = 0;
  int w = 0;
  do{
    
    if(strcmp(buf,"") == 0) continue;
    
    if(buf[0] == '\\'){
      if(n>0){
	// check ngram length
	if(_debug) printf("count=%d, head=%d\n", pos, _ngram_size[n-1]);
	if(pos != _ngram_size[n-1]){
	  std::cerr << "ngram size error" << std::endl;
	  exit(0);
	}else{
	  _ngram_size[n-1] = pos;
	}
      }
      if(strncmp(buf, "\\end\\",5) == 0){
	break;
      }else{
	sscanf(buf,"\\%d-grams:",&n);
	if (_debug) printf("read %d-gram\n", n);
	pos = 0;
	continue;
      }
    }
    
    if(_debug) std::cout << buf << std::endl;
    std::vector<std::string> column = split("\t", toString(buf));
    std::vector<std::string> word_array = split(" ", column[1]);
    prob  = atof(column[0].c_str()); // Log Probability
    alpha  = (int)column.size() == 3 ? atof(column[2].c_str()) : 0.0; // Log Backoff Weight
    
    if(n==1){
      _id2word[pos] = word_array[n-1];
      _word2id[word_array[n-1]] = pos;
      w++;
    }
    if(_debug) printf("word -> id, %s -> %d\n", word_array[n-1].c_str(), _word2id[word_array[n-1]]);
    // add Ngram Vectors
    //_nodes[n-1][pos].id = _word2id[word_array[n-1]];
    //_nodes[n-1][pos].prob = prob;
    //_nodes[n-1][pos].alpha = alpha;
    //_nodes[n-1][pos].nextpos = -1;
    
    //NgramNode nd;
    NgramNode nd(_word2id[word_array[n-1]], prob, alpha);
    _nodes[n-1][pos] = nd;
    if(n>1)
      _putNgramNodeData(word_array, _nodes[0], w, 1, n, pos); // wordarray, 追加node, nelem, 1gram, target_ngram, pos
    
    if(n>1){
      double p = _getProb0(word_array, n); // wordarray, 追加node, nelem, 1gram, target_ngram, pos
      if(_debug) std::cout << "search node: " << toString(p) << std::endl;
    }
    pos++;
  } while(ifs && ifs.getline(buf, sizeof(buf)));
  
  if (_debug) std::cout << "end" << std::endl;
  
  ifs.close();
  return 0;
}
