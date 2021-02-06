#include "decoder/Dic.h"

Dic::Dic(){}

Dic::~Dic(){}

int Dic::read(const char *filename){
  std::ifstream ifs;
  std::string line;
  std::vector<std::string> col;
  ifs.open(filename);
  if (ifs.fail()) return 1;
  while(ifs && std::getline(ifs, line)){
    col = split("\t", line);
    if (col.size() != 2) return 1;
    this->add_word(col[0].c_str(), col[1].c_str());
  }
  build_trie();
  return 0;
}
void Dic::add_word(const char *key, const char *features){ // word, read
  //printf("q2t=%s,%s\n",key, features);
  std::string k = key;
  std::string f = features;
  //this->query2tokens.insert(make_pair(k,f)); // add map
  query2tokens[k].emplace(f); // add map
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

int Dic::save(const char *filename){
  LOG(INFO) << "save dic=" << filename;
  //trie.save(filename);
  std::ofstream ofs;
  ofs.open(filename);
  if (ofs.fail()) return 1;
  std::unordered_map<std::string, std::unordered_set<std::string> >::iterator it;
  std::unordered_set<std::string>::iterator it_set;
  for(it=query2tokens.begin();it!=query2tokens.end();++it){
    std::string key=(*it).first;
    std::unordered_set< std::string > feat_set = (*it).second;
    for(it_set=feat_set.begin();it_set!=feat_set.end();++it_set){
      ofs << key << "\t" << *it_set << std::endl;
    }
  }
  ofs.close();
  return 0;
}

std::vector<std::string> Dic::lookupDic(const std::string &str) const{
  std::vector<std::string> retval;
  marisa::Agent agent;
  agent.set_query(str.c_str());
  while (this->trie.common_prefix_search(agent)) {

    retval.emplace_back(toString(agent.key().ptr()).substr(0, agent.key().length()));
    //std::cout.write(agent.key().ptr(), agent.key().length());
    //std::cout << ": " << agent.key().id() << std::endl;
  }
  return retval;
}

std::vector<std::string> Dic::getRead(std::string& key){
  std::vector<std::string> retval(query2tokens[key].begin(),query2tokens[key].end() );
  return retval;
}

bool Dic::is_exist(const std::string& str){
  if (query2tokens.size() == 0 ) false;
  return query2tokens.count(str)>0 ? true : false;
}

bool Dic::is_exist(const std::string& word, const std::string& read){
  if (query2tokens.size() == 0 ) false;
  if (query2tokens.count(word)>0) {
    if (query2tokens[ word ].count(read)>0) {
      return true;
    }
  }
  return false;
}
