/**
 * @file FileChunker.h
 * @brief Split and get a chunk data for large size file
 */
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
  FileChunker();
  ~FileChunker();
  int splitFile(const char* infile,const char* outdir, int n_split_line);
  int getChunkNum();
  void shuffleChunk();
  std::string basename(const std::string& path);
  std::vector<std::string> getChunkSamples();
  //std::string getRandomSample();
  template<class T> void shuffle(T ary[],int size);

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
