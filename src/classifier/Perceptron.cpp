#include "classifier/Perceptron.h"

Perceptron::Perceptron():
  _verbose(false),
  _learning_rate(0.1)
{}

Perceptron::~Perceptron(){}

int Perceptron::setModel(const char* modelfile){
  return Model::read(modelfile,_weights);
}

double Perceptron::get_score(const std::vector<std::string> &feat_vec){
  return classify(feat_vec);
}

// w_{i+1} = w_{i} + y * x * learning_rate
void Perceptron::fit(const std::vector<std::string> &feat_vec, double label){
  std::vector<std::string>::const_iterator feat = feat_vec.begin();
  for(size_t i=0;i<feat_vec.size();i++){
    if(_weights.count(feat_vec[i])>0){
      _weights[feat_vec[i]] += label*_learning_rate;
    }else{
      _weights[feat_vec[i]] = label*_learning_rate;
    }
    if(_verbose>0){
      printf("[DEBUG]%s:%s:%d:update score:%s\t%f\n",__FILE__,__FUNCTION__,__LINE__,feat_vec[i].c_str(), _weights[feat_vec[i]]);
    }
  }
}

// x * w # dotproduct
double Perceptron::classify(const std::vector<std::string> &feat_vec){
  double score = 0.0;
  std::vector<std::string>::const_iterator feat = feat_vec.begin();
    
  while(feat != feat_vec.end()){
    if(_weights.count(*feat)>0){
      score += _weights[*feat];
    }
    feat++;
  }
  return score;
}

int Perceptron::save(const char *outmodel){
  return Model::save(outmodel,_weights);
}

int Perceptron::read(const char* filename){
  return Model::read(filename,_weights);
}
