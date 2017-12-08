#ifndef _SVM_H_
#define _SVM_H_
//
// Support Vector Machine
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include "classifier/Model.h"
#undef _DEBUG_
#define _DEBUG_ 1
//using namespace std;
//using namespace std::tr1;

class SVM : public Model {
 public:
 SVM() : _lambda(1.0e-22),
    _penalty(0.5),
    _learning_rate(0.1),
    _updated_count(0){};
  ~SVM(){
  };
  void setModel(const char* modelfile){
    read(modelfile);
  }
  void fit(const std::vector<std::string> &feat_vec, double label);
  double classify(const std::vector<std::string> &feat_vec);
  double get_score(const std::vector<std::string> &feat_vec){
    return classify(feat_vec);
  }
  void add_update_count(){_updated_count++;}
  void save(const char *outmodel);
  void read(const char* filename);
  double margin(const std::vector<std::string> &feat_vec, double label);
  void l1_regularize(const std::vector<std::string> feat_vec);
  void regularize_all();

  // FOBOS L1 regularize // like sign method
  
 private:
  double _lambda;
  double _penalty;
  double _learning_rate;
  int _updated_count;
  std::unordered_map<std::string,double> _weights;    // feature -> weight
  std::unordered_map<std::string, int> _last_updated; // feature -> count 
  int sign(int x){ // sign関数
    return x>=0 ? 1 : -1;
  };
  // FOBOS L1 regularize // like sign method
  double clip(double a, double b){
    return sign(a)*std::max<double>(fabs(a)-b, 0);
  }
};


#endif
