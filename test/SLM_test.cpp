#include "ngram/SLM.h"

int main(){
  const char* arpaFName="sample_data/sample.3gram.arpa";
  SLM slm;
  slm.readLM(arpaFName);

  return 0;
}
