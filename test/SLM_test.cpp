#include "ngram/SLM.h"

int main(){
  const char* arpaFName="sample_data/sample.3gram.arpa";
  SLM slm;
  slm.readLM(arpaFName);
  std::cout << "ngram order:" << slm.getNgramLength() << std::endl;
  for(int i=0;i<slm.getNgramLength();i++){
    std::cout << i+1 << "-gram order:" << slm.getNgramSize(i) << std::endl;
  }


  return 0;
}
