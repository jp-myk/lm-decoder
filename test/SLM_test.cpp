#include "SLM.h"

int main(){
  const char* arpaFName="sample_set/train.3gram.arpa";
  SLM slm;
  slm.readLM(arpaFName);

  return 0;
}
