#ifndef _FILE_CHUNKER_H_
#define _FILE_CHUNKER_H_
#include "StringUtil.h"
#include "LOG.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>

#define RANDOM_SEED 711

class FileChunker{
 public:
  FileChunker(){
    //_num_sample     = 0;
    _num_split_file = 0;
    _num_split_line = 0;
    _rand_seed = RANDOM_SEED;
    std::srand(_rand_seed++);
  }
  ~FileChunker(){
    delete [] file_idx;
    //delete [] line_idx;
  

  }
  int splitFile(const char* infile,const char* outdir, int n_split_line);
  int getChunkNum(){
    return _num_split_file;
  }
  void shuffleChunk(){
    std::srand(_rand_seed++);
    _curr_file_id=0;
    shuffle<int>(file_idx,_num_split_file);
  }
  std::string basename(const std::string& path) {
    return path.substr(path.find_last_of('/') + 1);
  }
  std::vector<std::string> getChunkSamples();

  //std::string getRandomSample();
  template<class T> void shuffle(T ary[],int size){
    for(int i=0;i<size;i++){
      int j = rand()%size;
      T t = ary[i];
      ary[i] = ary[j];
      ary[j] = t;
    }
  }
 private:
  std::string buf;
  std::string _outdir;
  std::string _prefix;
  std::vector<std::string> _samples;
  int *file_idx;
  int *line_idx;
  unsigned int _rand_seed;
  int _num_all_sample;
  int _num_chunk_sample;
  int _curr_file_id;
  int _num_split_file;
  int _num_split_line;

};




#endif
