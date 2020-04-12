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
#include <unordered_map>
#include <unordered_set>
#include "marisa.h"
#include "utils/StringUtil.h"
#include "utils/LOG.h"
using namespace std;


class Dic{
 public:
  Dic();
  ~Dic();
  void read(const char *filename);
  void save(const char *filename);
  void update_trie(const char *key, const char* feature);
  std::vector<std::string> getRead(string &key);
  std::vector<std::string> lookupDic(const std::string &str);
  bool is_exist(const std::string& str);
  bool is_exist(const std::string& word, const std::string& read);

 private:
  void add_word(const char *key, const char* feature);
  void build_trie();
  marisa::Trie trie;
  marisa::Keyset keyset;
  std::unordered_map<std::string, std::unordered_set<std::string> > query2tokens;
  std::unordered_set<std::string> feat;
};


#endif
