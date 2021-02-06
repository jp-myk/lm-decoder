#ifndef _SVM_DECODER_H_
#define _SVM_DECODER_H_
#include <climits>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "decoder/Decoder.h"
#include "classifier/SVM.h"
#include "utils/StringUtil.h"
#include "utils/LOG.h"
#define DEBUG 0

class SVMDecoder : public Decoder {
 public:
  Lattice _word_lattice;
  Dic* _dic;
  SVM* _model;
  
  SVMDecoder();
  virtual ~SVMDecoder();
  int setDic(const char* dicfile);
  int setModel(const char* modelfile);
  int setDic(Dic* dic);
  int setModel(SVM* model);
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
  bool _loadDicFromFile;
  bool _loadModelFromFile;
};

#endif

