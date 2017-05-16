#include "Model.h"

void Model::save(const char* filename){
  printf("[DEBUG]%s:%s:%d:save model=%s\n",__FILE__,__FUNCTION__,__LINE__,filename);

  FILE *fp;
  std::tr1::unordered_map<std::string, double>::const_iterator it;
  if ((fp = fopen(filename, "w")) == NULL) {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  
  for(it=_weights.begin();it!=_weights.end();it++){
    fprintf(fp, "%s\t%.8f\n", (*it).first.c_str(), (*it).second);
  }
  fclose(fp);
}

void Model::read(const char* filename){
  std::ifstream ifs(filename);
  std::string line;
  while(ifs && std::getline(ifs, line)){
    std::vector<std::string> col=split("\t", line);
    _weights[col[0]] = atof(col[1].c_str());
  }
}


void Model::save(const char* filename,
		 const std::tr1::unordered_map<std::string, double>& weights){
  LOG(DEBUG) << "save model=" << filename;
  FILE *fp;
  std::tr1::unordered_map<std::string, double>::const_iterator it;
  if ((fp = fopen(filename, "w")) == NULL) {
    printf("file open error!!\n");
    exit(EXIT_FAILURE);
  }
  
  for(it=weights.begin();it!=weights.end();it++){
    fprintf(fp, "%s\t%.8f\n", (*it).first.c_str(), (*it).second);
  }
  fclose(fp);
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



void Model::read(const char* filename,
		 std::tr1::unordered_map<std::string, double>& weights){
  std::ifstream ifs(filename);
  std::string line;
  while(ifs && std::getline(ifs, line)){
    std::vector<std::string> col=split("\t", line);
    weights[col[0]] = atof(col[1].c_str());
  }
}
