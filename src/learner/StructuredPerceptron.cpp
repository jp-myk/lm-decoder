#include "learner/StructuredPerceptron.h"

StructuredPerceptron::StructuredPerceptron():
  _model(nullptr),
  _dic(nullptr)
  {}

StructuredPerceptron::~StructuredPerceptron(){
  if(_model!=nullptr) delete _model;
  if(_dic!=nullptr) delete _dic;
  _model = nullptr;
  _dic   = nullptr;
}

void StructuredPerceptron::setDic(const char* dicfile){
  _dic = new Dic();
  _dic->read(dicfile);
  decoder.setDic(_dic);
}

void StructuredPerceptron::setModel(const char* modelfile){
  _model = new Perceptron();
  _model->read(modelfile);
  decoder.setModel(_model);
}

void StructuredPerceptron::learn(const char* trainfile, const char* outdir,
				 int chunk_num,int iter_num){
  FileChunker fileChunker;
  std::vector<std::string>::iterator it;
  // make chunk data from large data
  fileChunker.splitFile(trainfile, outdir, chunk_num);
  
  
  for(int it_n=0;it_n<iter_num;it_n++){   // loop iteration
    for(int i=0;i<fileChunker.getChunkNum();i++){ // loop chunk
      std::vector<std::string> samples = fileChunker.getChunkSamples();
      //LOG(DEBUG) << "sample size=" << samples.size();
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

void StructuredPerceptron::learn(const std::string& input){
  Result corr_result = ILearner::parseLine(input); 
  checkUpdateDic(corr_result);
  std::string str = ILearner::joinKeyStr(corr_result); // input key join

  //LOG(DEBUG) << "input Surface=" << str; 
  Result hyp_result = decoder.decode(str);  // do decode!
  // if decode's result is NOT correct, update parameter 
  if(! (corr_result.surfaceList == hyp_result.surfaceList &&
	corr_result.readingList == hyp_result.readingList)){
    update_parameters(corr_result, hyp_result);
  }else{
    update_correct_count();
  }
  update_sample_count();
}

void StructuredPerceptron::update_parameters(Result &corr_result, 
					     Result &hyp_result){
  //LOG(DEBUG) << ":\tupdate_parameters_body: sentence";
  update_parameters_body(corr_result, 1.0);
  //LOG(DEBUG) << ":\tupdate_parameters_body: -1 * result";
  update_parameters_body(hyp_result, -1.0);
}

void StructuredPerceptron::update_parameters_body(Result &result, double label){
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
void StructuredPerceptron::update_node_score(const Node &node, double label){
  std::vector<std::string> f_vec = _feature.extract_node_feature(node);
  _model->fit(f_vec, label);
  //printf("[DEBUG]%s:%s:%d:%s,label=%f\n",__FILE__,__FUNCTION__,__LINE__,"update_node_score",label);
}

// 特徴量の追加,更新　
// Sword<tab>Sword
void StructuredPerceptron::update_edge_score(const Node &prev_node, const Node &node, double label){
  if (prev_node.empty()) return;
  std::vector<std::string> f_vec = _feature.extract_edge_feature(prev_node,node);
  _model->fit(f_vec, label);
  //printf("[DEBUG]%s:%s:%d:%s,label=%f\n",__FILE__,__FUNCTION__,__LINE__,"update_edge_score",label);
}


void StructuredPerceptron::save(const char *outdic, const char *outmodel){
  _dic->save(outdic);
  _model->save(outmodel);
}

void StructuredPerceptron::read(const char* filename){
  _model->read(filename);
}




/**
 * @brief  if exist unkown word in train data, add dictionary
 * @param  Result
 * @return void
 */
void StructuredPerceptron::checkUpdateDic(Result& correct_result){
  for(size_t i=0;i<correct_result.surfaceList.size();i++){
    if(!_dic->is_exist(correct_result.surfaceList[i],correct_result.readingList[i])){
      LOG(DEBUG)<< "update trie" << correct_result.surfaceList[i] << "\t"
		<< correct_result.readingList[i];
      _dic->update_trie(correct_result.surfaceList[i].c_str(),correct_result.readingList[i].c_str());
    }
  }
}



