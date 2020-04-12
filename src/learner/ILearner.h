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
  ILearner();
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
  /**
   * @brief   pair<表記,読み>のvector型を<Node>のvector型に変換
   * @param   pair<表記,読み> vector配列
   * @return  pair<Node> vector配列
   */
  std::vector<Node> convert_to_nodes(Result& result);
  /**
   * @brief  <表記,読み>pair vectorから読みを返す
   * @param  sentence <表記, 読み>pairのvector
   * @return 表記文字列
   */
  std::string joinKeyStr(Result& sentence);
  
  /**
   * @param  string: 明日/あした は/は 晴れ/はれ
   * @return vector: [ [明日,あした], [は,は], [晴れ,はれ] ]
   */
  Result parseLine(const std::string& line);
  void update_sample_count();
  void update_correct_count();
  void update_iter_count();
  void print_info();
 private:
  std::string _delimiter;
  int _num_correct;
  int _num_sample;
  int _num_iter;
};


#endif

