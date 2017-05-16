#ifndef _INTERFACE_DECODER_H_
#define _INTERFACE_DECODER_H_
#include <string>
#include <vector>
#include "classifier/Model.h"
#include "decoder/Dic.h"
#include "decoder/Lattice.h"
using namespace std;
using namespace std::tr1;

struct Result{
  std::vector<std::string> surfaceList;
  std::vector<std::string> readingList;
  double score;
};
typedef std::vector<Result> NBestResult;

struct Arc{
  int src_id;
  int dest_id;
  double g; // backward += forward + backward
  double f; // forward
  double h; // backward
  Node node;
};
  



class IDecoder{
 public:
  IDecoder():_debug(false){};
  virtual ~IDecoder(){};
  virtual void setDic(const char* dicfile) = 0;
  virtual void setModel(const char* modelfile) = 0;
  virtual void setDic(Dic* dic) = 0;
  virtual void setModel(Model* model) = 0;
  virtual Lattice& generate_lattice(Dic* dic, const std::string& str)=0;
  virtual Lattice& viterbi(Lattice &lattice) = 0;
  virtual NBestResult backward_a_star(Lattice& lattice, int n_best=100) = 0;
  virtual double get_node_score(const Node& node) = 0;
  virtual double get_edge_score(const Node& prev_node, const Node& node)=0;
  //virtual std::vector<std::string> analyze(const std::string& input)=0;
  //virtual std::vector<std::string> getNBestHyp()=0;  
  void debug(){_debug=true;}
  bool is_debug(){return _debug;}

 private:
  bool _debug;
  
};
#endif

