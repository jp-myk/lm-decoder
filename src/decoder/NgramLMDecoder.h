#ifndef _NGRAM_LM_DECODER_H_
#define _NGRAM_LM_DECODER_H_
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include <unordered_map>
#include <memory>
#include "ngram/SLM.h"
#include "decoder/Decoder.h"

class NgramLMDecoder : public Decoder{
 private:
  std::unique_ptr<Dic> _dic;
  Lattice _word_lattice,_hyp_lattice;
  SLM _slm;
  bool _debug;
  size_t _beam_size;
  std::string _delimiter;
  double get_lm_score(const Node *node, int ngram_size);
  
 public:
  NgramLMDecoder();
  virtual ~NgramLMDecoder();
  int setDic(const char* dicfile);
  int setModel(const char* lmfile);
  Result decode(const std::string& str);
  Lattice& forward(const Lattice &word_lattice); // beam search
  Result backtrace(const Lattice &hyp_lattice, int n_best=0);
  NBestResult getNBestHyp(int num_nbest);
};


#endif

