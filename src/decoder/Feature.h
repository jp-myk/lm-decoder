#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "decoder/Lattice.h"
#include "utils/StringUtil.h"

class Feature{
 public:
  Feature(){};
  ~Feature(){};
  //void add(string feature, double value);
  //std::unordered_set<std::string> node_features;
  //std::unordered_set<std::string> edge_features;
  std::unordered_map<std::string,double> weights;
  void save(const char* filename);
  void read(const char* filename);
  std::vector<std::string> node_feature(Node& node);
  std::vector<std::string> edge_feature(Node& prev_node, Node& node);
  std::vector<std::string> extract_node_feature(const Node& node);
  std::vector<std::string> extract_edge_feature(const Node& prev_node, const Node& node);

  //privete:
  
};


#endif
