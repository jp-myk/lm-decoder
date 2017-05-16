#ifndef _DIC_H_
#define _DIC_H_
/*
 * Dic
 * basically Read -> Kanji, but kanji -> read
 */
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "marisa.h"
#include "utils/StringUtil.h"
#include "utils/LOG.h"
using namespace std;
using namespace std::tr1;

class Dic{
 public:
  Dic(){};
  ~Dic(){};
  void read(const char *filename);
  void save(const char *filename);
  void update_trie(const char *key, const char* feature);
  vector<string> getRead(string &key);
  vector<string> lookupDic(const std::string &str);
  bool is_exist(const std::string& str){
    return query2tokens.count(str)>0 ? true : false;
  }
  bool is_exist(const std::string& word, const std::string& read){
    if(query2tokens.count(word)>0){
      if(query2tokens[ word ].count(read)>0){
	return true;
      }
    }
    return false;
  }
 private:
  void add_word(const char *key, const char* feature);
  void build_trie();
  marisa::Trie trie;
  marisa::Keyset keyset;
  std::tr1::unordered_map<std::string, std::tr1::unordered_set<std::string> > query2tokens;
  std::tr1::unordered_set<std::string> feat;
};


#endif
