#ifndef _DECODER_H_
#define _DECODER_H_
#include "utils/StringUtil.h"
#include "decoder/Feature.h"
#include "decoder/IDecoder.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#define _DEBUG_ 0
//typedef vector<string, string> 
using namespace std;



class Decoder : public IDecoder{
 public:
  Lattice _word_lattice;
  Dic *_dic;
  Model *_model;
  Decoder();
  virtual ~Decoder();
  void setDic(const char* dicfile);
  void setModel(const char* modelfile);
  void setDic(Dic *dic);
  void setModel(Model *model);
  /**
   * @brief 1-Best Decode
   */
  virtual Result decode(const std::string& input);
  /**
   * @brief N-Best Decode
   */
  virtual NBestResult getNBestHyp(int num_nbest);
  virtual Result backtrace(const Lattice &hyp_lattice, int n_best=0);
  Lattice& generate_lattice(Dic* dic, const std::string& str);
  Lattice& viterbi(Lattice &word_lattice);  
  NBestResult backward_a_star(Lattice& lattice, int n_best=100);
  double get_node_score(const Node& node);
  double get_edge_score(const Node& prev_node, const Node& node);
 
 private:
  Feature _feature;
  std::string _delimiter;
};



#endif

