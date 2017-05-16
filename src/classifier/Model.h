#ifndef _MODEL_H_
#define _MODEL_H_
/*
 * Linear Discrinant Model
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "utils/StringUtil.h"
#include "utils/LOG.h"

using namespace std;
using namespace std::tr1;


class Model{
 public:
  Model(){};
  virtual ~Model(){};
  std::tr1::unordered_map<std::string, double> _weights;
  virtual void fit(const std::vector<std::string>& feat_vec, double label){};
  virtual double classify(const std::vector<std::string>& feat_vec);
  virtual double get_score(const std::vector<std::string>& feat_vec);
  virtual void save(const char* filename);
  virtual void read(const char* filename);
  void save(const char* filename,
	    const std::tr1::unordered_map<std::string, double>& weights);
  void read(const char* filename,
	    std::tr1::unordered_map<std::string, double>& weights);
  
};

#endif
