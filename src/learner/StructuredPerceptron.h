#ifndef _S_PERCEPTRON_H_
#define _S_PERCEPTRON_H_
//
// Structured Perceptron Learner
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "learner/ILearner.h"
#include "decoder/Decoder.h"
#include "classifier/Perceptron.h"
#include "utils/LOG.h"
#include "utils/FileChunker.h"
#undef _DEBUG_
#define _DEBUG_ 0


class StructuredPerceptron: public ILearner{
 public:
  StructuredPerceptron(){
    _model = (Perceptron*)NULL;
    _dic = (Dic*)NULL;
  };
  virtual ~StructuredPerceptron(){
    if(_model!=(Perceptron*)NULL) delete _model;
    if(_dic!=(Dic*)NULL) delete _dic;
    _model = (Perceptron*)NULL;
    _dic   = (Dic*)NULL;
    decoder.setModel(_model);
    decoder.setDic(_dic);
  };
  void setDic(const char* dicfile){
    _dic = new Dic();
    _dic->read(dicfile);
    decoder.setDic(_dic);
  }
  void setModel(const char* modelfile){
    _model = new Perceptron();
    _model->read(modelfile);
    decoder.setModel(_model);
  }
  void learn(const char* trainfile, const char* outdir, int chunk_num,int iter_num){
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
      print_precision(); 
      update_iter_count();
      fileChunker.shuffleChunk();
    }
  }
  void learn(const std::string& corr_sentnece);
  void save(const char* outdic, const char *outmodel);
  void read(const char* filename);
 private:
  double learning_rate;
  void update_parameters(Result& corr_result, 
			 Result& hyp_result);
  void update_parameters_body(Result& result, double label);
  void update_node_score(const Node &node, double label);
  void update_edge_score(const Node &prev_node, const Node &node, double label);
  void checkUpdateDic(Result& correct_result);

  int sign(int x); // sign関数
  Decoder decoder;
  Dic *_dic;
  Model *_model;
  Feature _feature;

  //std::vector<std::string> convert_to_gold_standard(std::vector< std::pair<std::string,std::string> >& sentence);
};

#endif
