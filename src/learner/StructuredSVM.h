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
#include <memory>
#include "learner/ILearner.h"
#include "decoder/SVMDecoder.h"
#include "classifier/SVM.h"
#include "utils/LOG.h"
#include "utils/FileChunker.h"
#define _DEBUG_ 1


class StructuredSVM : public ILearner {
 public:
  StructuredSVM();
  virtual ~StructuredSVM();
  int setDic(const char* dicfile);
  int setModel(const char* modelfile);
  void learn(const char* trainfile, const char* outdir, int chunk_num,int iter_num);
  void learn(const std::string& corr_sentnece);
  int save(const char* outdic, const char *outmodel);
  int read(const char* filename);

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
  Feature _feature;
  std::unique_ptr<Dic> _dic;
  std::unique_ptr<SVM> _model;
};


#endif
