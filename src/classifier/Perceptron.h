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
  void setModel(const char* modelfile);
  void fit(const std::vector<std::string>& feat_vec, double label);
  double classify(const std::vector<std::string> &feat_vec);
  double get_score(const std::vector<std::string> &feat_vec);
  void save(const char* outdic, const char *outmodel);
  void save(const char *outmodel);
  void read(const char* filename);
 private:
  std::unordered_map<std::string,double> _weights;
  double _learning_rate;
  int sign(int x); // sign関数
  int _verbose;
};

#endif

