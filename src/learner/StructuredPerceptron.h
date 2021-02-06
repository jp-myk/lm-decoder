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
#include <memory>
#include "learner/ILearner.h"
#include "decoder/Decoder.h"
#include "classifier/Perceptron.h"
#include "utils/LOG.h"
#include "utils/FileChunker.h"
#undef _DEBUG_
#define _DEBUG_ 0


class StructuredPerceptron: public ILearner{
 public:
  StructuredPerceptron();
  virtual ~StructuredPerceptron();
  int setDic(const char* dicfile);
  int setModel(const char* modelfile);
  void learn(const char* trainfile, const char* outdir, int chunk_num,int iter_num);
  void learn(const std::string& corr_sentnece);
  int save(const char* outdic, const char *outmodel);
  int read(const char* filename);
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
  std::unique_ptr<Dic> _dic;
  std::unique_ptr<Model> _model;
  Feature _feature;

  //std::vector<std::string> convert_to_gold_standard(std::vector< std::pair<std::string,std::string> >& sentence);
};

#endif
