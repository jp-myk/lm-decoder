#include "FileChunker.h"
#include <iostream>

int main(){
  FileChunker fileChunker;
  fileChunker.splitFile("WDB/train.100.cps","tmp_dir",30);
  int iter_num = 2;
  for(int it_n=0;it_n<iter_num;it_n++){
    std::cout << "iter num=" << it_n+1 << std::endl;
    for(int i=0;i<fileChunker.getChunkNum();i++){
      std::vector<std::string> samples = fileChunker.getChunkSamples();
      std::cout << "file=" << i << "\tnum=" << samples.size() << std::endl;
    }
    fileChunker.shuffleChunk();
  }
  return 0;
}
