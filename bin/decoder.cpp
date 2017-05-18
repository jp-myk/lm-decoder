#include "decoder/Decoder.h"
#include <iostream>
#include <cstdio>
int main(int argc, char *argv[]){
  char *dicfile = argv[1];
  char *modelfile = argv[2];
  Decoder decoder;
  decoder.setDic(dicfile);
  decoder.setModel(modelfile);
  
  std::string line;
  while(std::getline(std::cin, line)){
    if(line=="") continue;
    Result result;
    std::vector<std::string> output;
    result = decoder.decode(line.c_str());
    for(size_t i=0;i<result.surfaceList.size();i++){
      std::string token = result.surfaceList[i]+":"+result.readingList[i]; 
      output.push_back(token);
    }
    std::cout << "======== 1-BEST =========" << std::endl;
    std::string outputStr = join(" ", output);
    printf("%d-best\t%s\t%.4f\n",1, outputStr.c_str(), result.score);

    std::cout << "======== N-BEST =========" << std::endl;
    NBestResult nbestResult;
    nbestResult = decoder.getNBestHyp(5);
    for(size_t n=0;n<nbestResult.size();n++){
      Result t_result = nbestResult[n];
      std::vector<std::string> t_output;
      for(size_t i=0;i<t_result.surfaceList.size();i++){
	std::string token = t_result.surfaceList[i]+":"+t_result.readingList[i]; 
	t_output.push_back(token);
      }
      outputStr = join(" ", t_output);
      printf("%d-best\t%s\t%.4f\n",(int)n+1, outputStr.c_str(), t_result.score);
    }

  }
  
  return 0;
}
