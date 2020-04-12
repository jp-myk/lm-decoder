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
  SVM();
  ~SVM();
  void setModel(const char* modelfile);
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

  
 private:
  double _lambda;
  double _penalty;
  double _learning_rate;
  int _updated_count;
  std::unordered_map<std::string,double> _weights;    // feature -> weight
  std::unordered_map<std::string, int> _last_updated; // feature -> count 
  /**
   * @brief sign function
   */
  int sign(int x);

  /**
   * @brief use in FOBOS L1 regularize
   */
  double clip(double a, double b);
};


#endif
