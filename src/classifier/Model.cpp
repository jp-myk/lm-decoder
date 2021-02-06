#include "Model.h"

int Model::save(const char* filename){
  LOG(INFO) << "save model=" << filename; 
  FILE *fp;
  std::unordered_map<std::string, double>::const_iterator it;
  if ((fp = fopen(filename, "w")) == NULL) {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  
  for(it=_weights.begin();it!=_weights.end();it++){
    fprintf(fp, "%s\t%.8f\n", (*it).first.c_str(), (*it).second);
  }
  fclose(fp);
}

int Model::read(const char* filename){
  struct stat statBuf;
  // if file exists, train from the beginging
  if (stat(filename, &statBuf) != 0) return 0;
  // if file not exist, train from the middle
  std::ifstream ifs(filename);
  std::string line;
  if (ifs.fail()) return 1;
  while(ifs && std::getline(ifs, line)){
    std::vector<std::string> col=split("\t", line);
    if (col.size() != 2) return 1;
    _weights[col[0]] = atof(col[1].c_str());
  }
  return 0;
}


int Model::save(const char* filename,
		 const std::unordered_map<std::string, double>& weights){
  LOG(INFO) << "save model=" << filename;
  FILE *fp;
  std::unordered_map<std::string, double>::const_iterator it;
  if ((fp = fopen(filename, "w")) == NULL) {
    printf("file open error!!\n");
    return 1;
  }
  
  for(it=weights.begin();it!=weights.end();it++){
    fprintf(fp, "%s\t%.8f\n", (*it).first.c_str(), (*it).second);
  }
  fclose(fp);
  return 0;
}

// x * w # dotproduct
double Model::classify(const std::vector<std::string> &feat_vec){
  double score = 0.0;
  std::vector<std::string>::const_iterator feat = feat_vec.begin();    
  while(feat != feat_vec.end()){
    //LOG(DEBUG) << *feat << std::endl;
    if(_weights.count(*feat)>0){
      score += _weights[*feat];
    }
    feat++;
  }
  return score;
}

// x * w # dotproduct
double Model::get_score(const std::vector<std::string> &feat_vec){
  double score = 0.0;
  std::vector<std::string>::const_iterator feat = feat_vec.begin();    
  while(feat != feat_vec.end()){
    //LOG(DEBUG) << *feat << std::endl;
    if(_weights.count(*feat)>0){
      score += _weights[*feat];
    }
    feat++;
  }
  return score;
}



int Model::read(const char* filename,
		std::unordered_map<std::string, double>& weights){
  std::ifstream ifs(filename);
  std::string line;
  if (ifs.fail()) return 1;
  while(ifs && std::getline(ifs, line)){
    std::vector<std::string> col=split("\t", line);
    if (col.size() != 2) return 1;
    weights[col[0]] = atof(col[1].c_str());
  }
  return 0;
}
