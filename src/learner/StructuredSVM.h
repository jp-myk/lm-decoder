#ifndef _S_SVM_H_
#define _S_SVM_H_
//
// Structured Support Vector Machine
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "learner/ILearner.h"
#include "decoder/SVMDecoder.h"
#include "classifier/SVM.h"
#include "utils/LOG.h"
#include "utils/FileChunker.h"
#define _DEBUG_ 1


class StructuredSVM : public ILearner {
 public:
  StructuredSVM(){
    _model = (SVM*)NULL;
    _dic = (Dic*)NULL;
    _num_sample  = 0;
    _num_correct = 0;
  };
  virtual ~StructuredSVM(){
    if(_model!=(SVM*)NULL) delete _model;
    if(_dic!=(Dic*)NULL) delete _dic;
    _model = (SVM*)NULL;
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
    _model = new SVM();
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
  std::unordered_map<int,Node> convert_to_gold_standard(Result& result);
  void checkUpdateDic(Result& correct_result);

  void update_parameters(Result& corr_result, 
			 Result& hyp_result);
  void update_parameters_body(Result& result, double label);
  void update_node_score(const Node &node, double label);
  void update_edge_score(const Node &prev_node, const Node &node, double label);

  void regularize_graph(Lattice& lattice);
  void regularize_all();
  void regularize_node(Node& node);
  void regularize_edge(Node& prev_node, Node& node);
  SVMDecoder decoder;
  Lattice *_lattice;
  Dic *_dic;
  Feature _feature;
  SVM *_model;
};


#endif
