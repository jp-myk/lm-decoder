#ifndef _I_LEARNER_H_
#define _I_LEARNER_H_
//
// Interface Learner
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "utils/StringUtil.h"
#include "decoder/IDecoder.h"

class ILearner{
 public:
  ILearner(){
    _delimiter   = ":";
    _num_correct = 0;
    _num_sample  = 0;
    _num_iter    = 1;

  };
  virtual ~ILearner(){};
  virtual void setDic(const char* dicfile){}
  virtual void setModel(const char* modelfile){}
  virtual void learn(const std::string& input){};
  virtual void save(const char* outdic, const char *outmodel){};
  virtual void read(const char* filename){};
  // private:
  //double learning_rate;
  virtual void update_parameters(Result &corr_result, 
				 Result &hyp_result){};
  virtual void update_parameters_body(Result& result,
				      double label){};
  virtual void update_node_score(Node &node, double label){};
  virtual void update_edge_score(Node &prev_node, Node &node, double label){};
  std::vector<Node> convert_to_nodes(Result& result);
  std::string joinKeyStr(Result& sentence);
  Result parseLine(const std::string& line);
  std::string _delimiter;
  void update_sample_count(){_num_sample++;}
  void update_correct_count(){_num_correct++;};
  void update_iter_count(){
    _num_correct = 0;
    _num_sample  = 0;
    _num_iter++;
  };
  void print_precision(){
    printf("iter=%d\tprecision=%f\n", _num_iter,(float)_num_correct/_num_sample);
  }
  int _num_correct;
  int _num_sample;
  int _num_iter;

};


#endif

