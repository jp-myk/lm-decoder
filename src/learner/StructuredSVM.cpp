#include "StructuredSVM.h"

StructuredSVM::StructuredSVM():
  _model(nullptr),
  _dic(nullptr){}

StructuredSVM::~StructuredSVM(){
  if(_model != nullptr) delete _model;
  if(_dic != nullptr) delete _dic;
  _model = nullptr;
  _dic   = nullptr;
}

void StructuredSVM::setDic(const char* dicfile){
  _dic = new Dic();
  _dic->read(dicfile);
  decoder.setDic(_dic);
}

void StructuredSVM::setModel(const char* modelfile){
  _model = new SVM();
  _model->read(modelfile);
  decoder.setModel(_model);
}

void StructuredSVM::learn(const char* trainfile, const char* outdir, int chunk_num,int iter_num){
  FileChunker fileChunker;
  std::vector<std::string>::iterator it;
  // make chunk data from large data
  fileChunker.splitFile(trainfile, outdir, chunk_num);
  
  for(int it_n=0;it_n<iter_num;it_n++){   // loop iteration
    for(int i=0;i<fileChunker.getChunkNum();i++){ // loop chunk
      std::vector<std::string> samples = fileChunker.getChunkSamples();
      for(it=samples.begin();it!=samples.end();it++){ // loop sample
	if(*it=="") continue;
	learn(*it); // train	  
      }
    }
    print_info(); 
    update_iter_count();
    fileChunker.shuffleChunk();
  }
}



/**
 * @brief StructuredSVM Fast FOBOS Learning
 * 損失化項の計算
 * 正則化項の計算
 * 高速化=普通に計算するとパラメータ次元数求める必要があるが、必要な部分だけ求める。
 * graphに出てくるところだけ正則化をかける
 *
 * 詳しくは Efficient Online and Batch Learning Using Forward Backward Splitting; John Duchi, Yoram Singer; Journal of Machine Learning Research 10(Dec):2899-2934, 2009 を参照。
 **/
void StructuredSVM::learn(const std::string& input){
  Result corr_result = ILearner::parseLine(input); 
  checkUpdateDic(corr_result);
  std::string str = ILearner::joinKeyStr(corr_result); // input key join
  std::unordered_map<int, Node> gold=convert_to_gold_standard(corr_result);

  //LOG(DEBUG) << " input Surface=" << str; 
  Lattice &lattice = decoder.generate_lattice(str);
  
  regularize_graph(lattice); // 必要になった部分だけ正則化をかける

  Result hyp_result = decoder.decode_with_penalty(str,gold);  // do decode!

  // if decode's result is NOT correct, update parameter 
  if(! (corr_result.surfaceList == hyp_result.surfaceList &&
	corr_result.readingList == hyp_result.readingList)){
    update_parameters(corr_result, hyp_result);
  }else{
    update_correct_count();
  }
  _model->add_update_count();
  update_sample_count();
}

void StructuredSVM::regularize_graph(Lattice& lattice){
  size_t lattice_size = lattice.sizeOfFrame();
  for(size_t i=1;i<lattice_size;i++){
    size_t node_size = lattice.sizeOfNodePerFrame(i);
    for(size_t j=0;j<node_size;j++){ // loop i-th frame's node
      Node *node = &lattice[i][j];
      if(node->is_bos()) continue;
      regularize_node(*node);
      int prev_pos = node->get_prev_pos();
      vector<Node>::iterator prev_node;
      size_t prev_node_size =  lattice.sizeOfNodePerFrame(prev_pos);
      for(size_t k=0;k<prev_node_size;k++){ // loop prev frame's node
	Node *prev_node = &lattice[prev_pos][k];
	regularize_edge(*prev_node, *node);
      }
    }
  }
}

void StructuredSVM::regularize_node(Node& node){
  std::vector<std::string> features=_feature.extract_node_feature(node);
  _model->l1_regularize(features);
}

void StructuredSVM::regularize_edge(Node& prev_node, Node& node){
  std::vector<std::string> features=_feature.extract_edge_feature(prev_node, node);
  _model->l1_regularize(features);
}

void StructuredSVM::regularize_all(){
  _model->regularize_all();
}


void StructuredSVM::update_parameters(Result &corr_result, 
					     Result &hyp_result){
  //LOG(DEBUG) << ":\tupdate_parameters_body: sentence";
  update_parameters_body(corr_result, 1.0);
  //LOG(DEBUG) << ":\tupdate_parameters_body: -1 * result";
  update_parameters_body(hyp_result, -1.0);
}

void StructuredSVM::update_parameters_body(Result &result, double label){
  std::vector<Node> nodes = ILearner::convert_to_nodes(result);
  std::vector<Node>::iterator node;
  Node prev_node;//(NULL,NULL,0);
  //prev_node->set((const char*)NULL,(const char*)NULL,0);
  int i=0;
  for(node=nodes.begin();node!=nodes.end();node++){
    update_node_score(*node, label);
    if(i>0) update_edge_score(prev_node, *node, label);
    prev_node = *node;
    i++;
  }
}

// 特徴量の追加,更新
// Sword
// Sword<tab>Rread
void StructuredSVM::update_node_score(const Node &node, double label){
  std::vector<std::string> f_vec = _feature.extract_node_feature(node);
  _model->fit(f_vec, label);
}

// 特徴量の追加,更新　
// Rread<tab>Rread
void StructuredSVM::update_edge_score(const Node &prev_node, const Node &node, double label){
  if (prev_node.empty()) return;
  std::vector<std::string> f_vec = _feature.extract_edge_feature(prev_node,node);
  _model->fit(f_vec, label);
  //printf("[DEBUG]%s:%s:%d:%s,label=%f\n",__FILE__,__FUNCTION__,__LINE__,"update_edge_score",label);
}


void StructuredSVM::save(const char *outdic, const char *outmodel){
  _model->regularize_all(); //全体の正規化
  _dic->save(outdic);
  _model->save(outmodel);
}

void StructuredSVM::read(const char* filename){
  _model->read(filename);
}


std::unordered_map<int,Node> StructuredSVM::convert_to_gold_standard(Result& result){
  std::unordered_map<int, Node> retval;
  std::vector<Node> nodes = ILearner::convert_to_nodes(result);
  std::vector<Node>::iterator it;
  int nodeSize=(int)nodes.size();
  for(int i=0;i<nodeSize;i++){
    retval[nodes[i].endpos]=nodes[i];
  }
  return retval;
}

/**
 * @brief  if exist unkown word in train data, add dictionary
 * @param  Result
 * @return void
 */
void StructuredSVM::checkUpdateDic(Result& correct_result){
  for(size_t i=0;i<correct_result.surfaceList.size();i++){
    if(!_dic->is_exist(correct_result.surfaceList[i],correct_result.readingList[i])){
      LOG(DEBUG)<< "update trie" << correct_result.surfaceList[i] << "\t"
		<< correct_result.readingList[i];
      _dic->update_trie(correct_result.surfaceList[i].c_str(),correct_result.readingList[i].c_str());
    }
  }
}


