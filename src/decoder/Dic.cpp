#include "decoder/Dic.h"

void Dic::read(const char *filename){
  std::ifstream ifs;
  std::string line;
  std::vector<std::string> col;
  ifs.open(filename);
  
  while(ifs && std::getline(ifs, line)){
    col = split("\t", line);
    this->add_word(col[0].c_str(), col[1].c_str());
  }
  build_trie();
  //LOG(DEBUG) << "dic read done.";
}
void Dic::add_word(const char *key, const char *features){ // word, read
  //printf("q2t=%s,%s\n",key, features);
  std::string k = key;
  std::string f = features;
  //this->query2tokens.insert(make_pair(k,f)); // add map
  //std::cout << "add map:" << k << "\t" << f<< std::endl;
  query2tokens[k].insert(f); // add map
  //std::cout << "add trie" << std::endl; 
  keyset.push_back(key);   // add Trie;
}

void Dic::update_trie(const char* word, const char* features){ // word, read
  LOG(DEBUG) << word << "\t" << features;
  add_word(word, features);
  build_trie();

}

void Dic::build_trie(){
  trie.build(this->keyset);
}

void Dic::save(const char *filename){
  LOG(DEBUG) << "save dic=" << filename;
  //trie.save(filename);
  std::ofstream ofs;
  ofs.open(filename);
  std::tr1::unordered_map<std::string, std::tr1::unordered_set<std::string> >::iterator it;
  std::tr1::unordered_set<std::string>::iterator it_set;
  for(it=query2tokens.begin();it!=query2tokens.end();++it){
    std::string key=(*it).first;
    std::tr1::unordered_set< std::string > feat_set = (*it).second;
    for(it_set=feat_set.begin();it_set!=feat_set.end();++it_set){
      ofs << key << "\t" << *it_set << std::endl;
    }
  }
  ofs.close();
  
}


vector<string> Dic::lookupDic(const string &str){
  vector<string> retval;
  marisa::Agent agent;
  agent.set_query(str.c_str());
  while (this->trie.common_prefix_search(agent)) {

    retval.push_back(toString(agent.key().ptr()).substr(0, agent.key().length()));
    //std::cout.write(agent.key().ptr(), agent.key().length());
    //std::cout << ": " << agent.key().id() << std::endl;
  }
  return retval;
}

vector<string> Dic::getRead(std::string& key){
  std::vector<std::string> retval(query2tokens[key].begin(),query2tokens[key].end() );
  return retval;
}
