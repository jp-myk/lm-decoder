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
  Decoder(): _delimiter(":"){
    _dic = (Dic*)NULL;
    _model = (Model*)NULL;
  };
  virtual ~Decoder(){
    if(_model!=(Model*)NULL) delete _model;
    if(_dic!=(Dic*)NULL) delete _dic;
    _model = (Model*)NULL;    
    _dic   = (Dic*)NULL;
  };
  void setDic(const char* dicfile){
    _dic = new Dic();
    _dic->read(dicfile);
  }
  void setModel(const char* modelfile){
    _model = new Model();
    _model->read(modelfile);
  }
  void setDic(Dic *dic){
    _dic = dic;
  }
  void setModel(Model *model){
    _model = model;
  }
  Lattice& generate_lattice(Dic* dic, const std::string& str);
  Lattice& viterbi(Lattice &word_lattice);
  virtual Result backtrace(const Lattice &hyp_lattice, int n_best=0);

  NBestResult backward_a_star(Lattice& lattice, int n_best=100);
  virtual Result decode(const std::string& input){
    Result result;
    Lattice& word_lattice = generate_lattice(_dic,input);
    Lattice& hyp_lattice = viterbi(word_lattice);
    //_word_lattice.dump();
    result = backtrace(hyp_lattice);
    return result;
  }

  virtual NBestResult getNBestHyp(int num_nbest){
    NBestResult nbestResult;
    nbestResult= backward_a_star(_word_lattice,num_nbest);
    int n_best = std::min(num_nbest,(int)nbestResult.size());
    return nbestResult;
  }
 
  double get_node_score(const Node& node);
  double get_edge_score(const Node& prev_node, const Node& node);
 
 private:
  Feature _feature;
  std::string _delimiter;
};



#endif

