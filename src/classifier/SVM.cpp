#include "classifier/SVM.h"

SVM::SVM():
  _lambda(1.0e-22),
  _penalty(0.5),
  _learning_rate(0.1),
  _updated_count(0)
{}

SVM::~SVM(){}

void SVM::setModel(const char* modelfile){
  read(modelfile);
}

double SVM::margin(const std::vector<std::string> &feat_vec,
		   double label){
  double score = 0.0;
  std::vector<std::string>::const_iterator feat = feat_vec.begin();
  while(feat != feat_vec.end()){
    if(_weights.count(*feat)>0){
      score += _weights[*feat];
    }
    feat++;
  }
  return score*label;
}


// w_{i+1} = w_{i} + y * x * learning_rate
void SVM::fit(const std::vector<std::string> &feat_vec, double label){
  for(size_t i=0;i<feat_vec.size();i++){
    if(_weights.count(feat_vec[i])>0){
      _weights[feat_vec[i]] += label*_learning_rate;
    }else{
      _weights[feat_vec[i]] = label*_learning_rate;
    }
    //printf("[DEBUG]%s:%s:%d:update score:%s\t%f\n",__FILE__,__FUNCTION__,__LINE__,feat_vec[i].c_str(), _weights[feat_vec[i]]);
  }
}

// x * w # dotproduct
double SVM::classify(const std::vector<std::string> &feat_vec){
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


void SVM::regularize_all(){
  //LOG(DEBUG) << "trainig done";
  std::vector<std::string> feat_vec;
  feat_vec.reserve(_weights.size());
  std::unordered_map<std::string, double>::iterator it;
  for(it=_weights.begin();it!=_weights.end();++it){
    feat_vec.push_back(it->first);
  }
  l1_regularize(feat_vec);
}

void SVM::l1_regularize(const std::vector<std::string> feat_vec){
  std::vector<std::string>::const_iterator feat = feat_vec.begin();
  for(size_t i=0;i<feat_vec.size();i++){
    if(_weights.count(feat_vec[i])>0){
      if(_last_updated.count(feat_vec[i])>0){
	int c = _updated_count - _last_updated[feat_vec[i]];
	
	double new_val=clip(_weights[feat_vec[i]], _lambda*c);
	if(fabs(new_val)<1.0e-10){
	  // or ERASE Feature!
	  _weights[feat_vec[i]] = 0.0;
	}else{
	  _weights[feat_vec[i]] = new_val;
	}
      }else{
	int c = _updated_count;
	_last_updated[feat_vec[i]] = c;
	_weights[feat_vec[i]] = clip(_weights[feat_vec[i]], _lambda*c);;  
      }
    }
  }
}


void SVM::save(const char *outmodel){
  regularize_all(); //全体の正規化
  Model::save(outmodel,_weights);
}

void SVM::read(const char* filename){
  Model::read(filename,_weights);
}

int SVM::sign(int x){
  return x>=0 ? 1 : -1;
};

double SVM::clip(double a, double b){
  return sign(a)*std::max<double>(fabs(a)-b, 0);
}
