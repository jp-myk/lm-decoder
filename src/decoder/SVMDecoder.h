#ifndef _SVM_DECODER_H_
#define _SVM_DECODER_H_
#include <climits>
#include <string>
#include <vector>
#include <unordered_map>
#include "decoder/Decoder.h"
#include "classifier/SVM.h"
#include "utils/StringUtil.h"
#include "utils/LOG.h"
#define DEBUG 1
//typedef vector<string, string> 
using namespace std;


class SVMDecoder : public Decoder {
 public:
  Lattice _word_lattice;
  Dic *_dic;
  SVM *_model;
  
  SVMDecoder(): _delimiter(":"){
    _dic = (Dic*)NULL;
    _model = (SVM*)NULL;
    _penalty=0.05;
  };
  virtual ~SVMDecoder(){
    if(_model!=(SVM*)NULL) delete _model;
    if(_dic!=(Dic*)NULL) delete _dic;
    _model = (SVM*)NULL;    
    _dic   = (Dic*)NULL;
  };
  void setDic(const char* dicfile){
    _dic = new Dic();
    _dic->read(dicfile);
  }
  void setModel(const char* modelfile){
    _model = new SVM();
    _model->read(modelfile);
  }
  void setDic(Dic *dic){
    _dic = dic;
  }
  void setModel(Model *model){
    _model =(SVM*)model;
  }
  Lattice& generate_lattice(const std::string& str){
    return Decoder::generate_lattice(_dic, str);
  }
  Lattice& viterbi_with_penalty(Lattice &word_lattice, std::unordered_map<int, Node>& gold);
  Result decode_with_penalty(const std::string &input, std::unordered_map<int, Node>& gold){
    Result result;
    Lattice &word_lattice = Decoder::generate_lattice(_dic, input);
    Lattice &hyp_lattice = viterbi_with_penalty(word_lattice, gold);
    //_word_lattice.dump();
    result = Decoder::backtrace(hyp_lattice);
    return result;
  }
 private:
  ;
  Feature _feature;
  std::string _delimiter;
  bool _is_correct_node(Node& node, std::unordered_map<int, Node>& gold);
  bool _is_correct_edge(Node& prev_node, Node& node, std::unordered_map<int, Node>& gold);
  double get_node_score(Node& node, std::unordered_map<int, Node>& gold);
  double get_edge_score(Node& prev_node, Node& node, std::unordered_map<int, Node>& gold);
  double _penalty;

};

#endif

