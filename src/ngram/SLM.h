//----------------------------------
//
// Language Model Data Structure
// ngram vector based on sorted array
//
//----------------------------------
#ifndef _SLM_H_
#define _SLM_H_
#include <string>
#include <vector>
#include <map>
#include <set>
#include <tr1/unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include "utils/ProbUtil.h"
#include "utils/StringUtil.h"
//#include "darts.h"
#define MAX_NGRAM 10
//using namespace std;
//using namespace std::tr1;



typedef int WordID;
typedef std::vector<std::string> Vocab;
typedef std::tr1::unordered_map<std::string, int> WORD2ID;
typedef std::tr1::unordered_map<int, std::string> ID2WORD;

struct NgramNode{
  NgramNode(WordID _id=-1, double _prob=0.0, double _alpha=0.0){
    id=_id;
    prob=_prob;
    alpha=_alpha;
    nextpos = -1;
    nelem = 0;
    pelem = 0;
  }
  WordID id;
  double prob;  // logProb
  double alpha; // logBOW
  int nextpos; // first pos number in next level (bsearch start)
  unsigned int nelem; // the number of element in next level (bsearch end) // and for witten bell technique
  unsigned int pelem; // the number of element in prev level (not yet implemented) // for kneser-ney technique

};

typedef struct status_t{
  int ngram_len;
  int hit[MAX_NGRAM];
  
}SLMStatus;

class SLM{
 private:
  int _ngram_len;
  Vocab vocab; // id -> word
  WORD2ID _word2id;
  ID2WORD _id2word;
  
  NgramNode **_nodes; // ngram expresses node array, node[0],node[1]
  int _ngram_size[MAX_NGRAM];
  int _putNgramNodeData(std::vector<std::string> &word_array, NgramNode *nd_vec, int nelem, int curr_level, int target_level, int pos);
  void _updateNgramNodeData(std::vector<std::string>& word_array, NgramNode *nn2);
  NgramNode* _search_node(std::vector<std::string> &word_array, NgramNode *nd_vec, 
			 int nelem, int curr_level, int target_level);
  std::vector<std::string> _dumpNgram0(int curr_level, int target_level, int pos, int nelem, std::string& buf);
  double _getProb0(std::vector<std::string>& vec, int len); // return Prob existing ngram
  std::string _getWordStr(int wordID);  
  //int addNgram(vector<string> word_array, NgramNode *ng);
  bool _debug;
 public:
  SLM(){
    _debug=false;
  };
  ~SLM(){
    int i;
    for(i=0;i<_ngram_len;i++){
      //for(int j=0;j<_ngram_size[i];j++){
	//free(_nodes[i]+j);
      //}
      //free(_nodes[i]);
      delete[] _nodes[i];
    }
    delete _nodes;
  };
  
  int readLM(const char *filename);
  int writeLM(char *filename);
  int getNgramLength();
  int getNgramSize(int n_order);
  double getProb(std::vector<std::string>& vec, int len);  // return Prob with BOW
  double getProb(std::vector<int>& vec, int len);  // return Prob with BOW
  double getProb(std::string& str, int len);  // return Prob with BOW
  double getPPL(std::vector<std::string>& word_array, int order);
  double getEntropy(std::vector<string>& vec, int len);  // return Prob with BOW
  bool isNode(std::vector<std::string>& vec);     // return existing ngramNode or not
  double getBOW(std::vector<std::string>& vec);   // return BackOff Weight
  
  vector<std::string> dumpNgram(int len);
  vector<std::string> getVocab();
  int getWordID(std::string& word);
  void debug(){_debug=false;}
};

  


#endif
