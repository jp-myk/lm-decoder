#include "decoder/Feature.h"


void Feature::save(const char* filename){
  FILE *fp;
  std::tr1::unordered_map<std::string, double>::iterator it;
  if ((fp = fopen(filename, "w")) == NULL) {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  
  for(it=weights.begin();it!=weights.end();it++){
    std::cout << (*it).first << "\t" << (*it).second << std::endl;
    fprintf(fp, "%s\t%.8f\n", (*it).first.c_str(), (*it).second);
  }
  fclose(fp);
}

void Feature::read(const char* filename){
  std::ifstream ifs(filename);
  std::string line;
  while(ifs && std::getline(ifs, line)){
    std::vector<std::string> col=split("\t", line);
    weights[col[0]] = atof(col[1].c_str());
  }
}

std::vector<std::string> Feature::extract_node_feature(const Node& node){
  std::vector<std::string> retval;
  std::string feat;
  //kana->kanji
  //feat = "S"+node.word;
  feat = "R"+node.read;
  retval.push_back(feat);
  //kana->kanji
  //feat = "S"+node.word+" R"+node.read;
  feat = "R"+node.read+" S"+node.word;
  retval.push_back(feat);
  return retval;
}

std::vector<std::string> Feature::extract_edge_feature(const Node& prev_node, const Node& node){
  std::vector<std::string> retval;
  if (prev_node.empty()) return retval;
  std::string feat;
  feat = "R"+prev_node.read+" R"+node.read;
  retval.push_back(feat);
  // kana->kanji
  //feat = "S"+prev_node.word+" S"+node.word;
  return retval;
}



std::vector<std::string> Feature::node_feature(Node& node){
  std::vector<std::string> retval;
  std::string feat;
  //kana->kanji
  //feat = "S"+node.word;
  feat = "R"+node.read;
  retval.push_back(feat);
  //kana->kanji
  //feat = "S"+node.word+" R"+node.read;
  feat = "R"+node.read+" S"+node.word;
  retval.push_back(feat);
  return retval;
}

std::vector<std::string> Feature::edge_feature(Node& prev_node, Node& node){
  std::vector<std::string> retval;
  if (prev_node.empty()) return retval;
  std::string feat;
  feat = "R"+prev_node.read+" R"+node.read;
  retval.push_back(feat);
  // kana->kanji
  //feat = "S"+prev_node.word+" S"+node.word;
  return retval;
}

