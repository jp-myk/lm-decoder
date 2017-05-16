#ifndef _NGRAM_LM_DECODER_H_
#define _NGRAM_LM_DECODER_H
#include "ngram/SLM.h"
#include "decoder/Decoder.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include <tr1/unordered_map>

class NgramLMDecoder : public Decoder{
 private:
  Dic *_dic;
  Lattice _word_lattice,_hyp_lattice;
  SLM _slm;
  bool _debug;
  int _beam_size;
  std::string _delimiter;
  double get_lm_score(const Node *node, int ngram_size);
  
 public:
  NgramLMDecoder(){
    _delimiter=":";
    _debug = false;
    _beam_size = 30;
    _dic = (Dic*)NULL;
    //_slm = (SLM*)NULL;
    
  };
  virtual ~NgramLMDecoder(){
    //if(_model!=(SLM*)NULL) delete _slm;
    if(_dic!=(Dic*)NULL) delete _dic;
    //_model = (SLM*)NULL;    
    _dic   = (Dic*)NULL;
  };
  void setDic(const char* dicfile){
    _dic = new Dic();
    _dic->read(dicfile);
  }
  void setModel(const char* lmfile){
    _slm.readLM(lmfile);
  }
  
  Result decode(const std::string& str);
  Lattice& forward(const Lattice &word_lattice); // beam search
  Result backtrace(const Lattice &hyp_lattice, int n_best=0);
  NBestResult getNBestHyp(int num_nbest);
};


#endif

