#include "decoder/NgramLMDecoder.h"
void usage(){
  std::cout << "USAGE:" << std::endl;
  std::cout << "./lmdecoder <DIC_FILE>  <ARPA_LM>" << std::endl;
  std::cout << std::endl;
  std::cout << "DIC_FILE Format:" << std::endl;
  std::cout << "WORD<tab>READ" << std::endl;
  std::cout << std::endl;
  exit(0);
}
int main(int argc, char *argv[]){
  std::string dicFName ="sample_set/train.dic";
  std::string arpaFName="sample_set/train.3gram.arpa";

  if(argc>1){
    if(!strcmp(argv[1], "-help") || !strcmp(argv[1], "--help")){
      usage();
    }
  }
  if(argc==3){
    dicFName =argv[1];
    arpaFName=argv[2];
  }

  NgramLMDecoder decoder;
  decoder.setDic(dicFName.c_str());
  decoder.setModel(arpaFName.c_str());
  
  std::string line;
  while(std::getline(std::cin, line)){
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
