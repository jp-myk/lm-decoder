#include "learner/ILearner.h"

/**
 * param  string: 明日/あした は/は 晴れ/はれ
 * return vector: [ [明日,あした], [は,は], [晴れ,はれ] ]
 */
Result ILearner::parseLine(const std::string& line){
  Result result;
  std::vector<std::string> surfaceList;
  std::vector<std::string> readingList;


  std::vector< std::pair<std::string, std::string> > retval;
  std::vector<std::string> tokens;
  std::vector<std::string>::iterator it;
  
  tokens = split(" ", line);
  for(it=tokens.begin();it!=tokens.end();it++){
    std::vector<std::string> kv=split(_delimiter.c_str(), *it);
    //retval.push_back( std::pair<std::string, std::string>(kv[0],kv[1]) );
    surfaceList.push_back(kv[0]);
    readingList.push_back(kv[1]);
  }
  result.surfaceList = surfaceList;
  result.readingList = readingList;
  result.score = 0.0;
  return result;
}

/**
 * @brief  <表記,読み>pair vectorから読みを返す
 * @param  sentence <表記, 読み>pairのvector
 * @return 表記文字列
 */
std::string ILearner::joinKeyStr(Result& result){
  std::string retval;
  for(size_t i=0;i<result.surfaceList.size();i++){
    retval += result.surfaceList[i]; 
  }
  return retval;
}

/*
 * @brief   pair<表記,読み>のvector型を<Node>のvector型に変換
 * @param   pair<表記,読み> vector配列
 * @return  pair<Node> vector配列
 */
std::vector<Node> ILearner::convert_to_nodes(Result& result){
  std::vector<Node> retval;
  std::vector< std::pair<std::string, std::string> >::iterator it;
  Node bos,eos,prev;
  int endpos=0;
  bos.set("<s>","", endpos+1); // 始端
  retval.push_back(bos);
  prev=bos;
  endpos++;
  for(size_t i=0;i<result.surfaceList.size();i++){
    //for(it=sentence.begin();it!=sentence.end();it++){
    endpos += strlen_utf8(result.surfaceList[i]);
    Node node(result.surfaceList[i], result.readingList[i], endpos+1);
    if(!prev.empty()) node.prev = &prev;
    retval.push_back(node);
    prev = node;
  }
  eos.set("</s>","", endpos+1);
  eos.prev = &prev;
  retval.push_back(eos);
  return retval;
}
