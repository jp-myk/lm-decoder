#ifndef _PERCEPTRON_H_
#define _PERCEPTRON_H_
//
// Perceptron 
//
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "classifier/Model.h"

class Perceptron : public Model{
 public:
  Perceptron();
  virtual ~Perceptron();
  int setModel(const char* modelfile);
  void fit(const std::vector<std::string>& feat_vec, double label);
  double classify(const std::vector<std::string> &feat_vec);
  double get_score(const std::vector<std::string> &feat_vec);
  int save(const char* outdic, const char *outmodel);
  int save(const char *outmodel);
  int read(const char* filename);
 private:
  std::unordered_map<std::string,double> _weights;
  double _learning_rate;
  int sign(int x); // sign関数
  int _verbose;
};

#endif

