#include "utils/FileChunker.h"

int FileChunker::splitFile(const char* infile, const char* outdir, int n_split_line){
  //int outfile_idx = 0;
  //int n_line = 0;
  std::ifstream ifs(infile);
  std::ofstream ofs;
  std::stringstream ss;

  _num_split_file = 0;
  _num_all_sample = 0;
  _num_split_line = n_split_line;

  _prefix = basename(infile);
  _outdir = outdir;
  struct stat st;
  if(stat(outdir, &st) != 0){
    if(mkdir(outdir, 0775) != 0){
      LOG(DEBUG) << "fail make dir:" << outdir;
      exit(1);
    }
  }
  
  ss << _outdir << "/" << _prefix << "." << _num_split_file;
  ofs.open(ss.str().c_str());

  std::string line;
  std::vector<std::string> samples;
  std::vector<std::string>::iterator it;

  while(ifs && std::getline(ifs, line)){
    samples.push_back(line);
    _num_all_sample++;

    if(_num_all_sample % _num_split_line == 0){
      for(it=samples.begin();it!=samples.end();it++){
	ofs << *it << std::endl;
      }
      LOG(INFO) << "file=" << _num_split_file << "\tsample=" << _num_all_sample;
      _num_split_file++;
      ofs.close();
      samples.clear();
      ss.str("");
      ss << outdir << "/" << _prefix << "." << _num_split_file;
      ofs.open(ss.str().c_str());
    }

  }

  if(samples.size()>0){
    for(it=samples.begin();it!=samples.end();it++){
	ofs << *it << std::endl;
    }
    LOG(INFO) << "file=" << _num_split_file << "\tsample=" << _num_all_sample;
    _num_split_file++;

  }


  ofs.close();
  ifs.close();

  file_idx = new int[_num_split_file];
  for(int i=0;i<_num_split_file;i++){file_idx[i]=i;}
  shuffleChunk();
  return _num_split_file;
}

std::vector<std::string> FileChunker::getChunkSamples(){
  std::stringstream ss;  
  std::ifstream ifs;
  std::string line;

  _samples.clear();
  ss << _outdir << "/" << _prefix << "." << file_idx[_curr_file_id];

  //LOG(DEBUG)<< ss.str();

  ifs.open(ss.str().c_str());
  while(ifs && std::getline(ifs, line)){
    _samples.push_back(line);
  }
  _num_chunk_sample = _samples.size();
  //shuffle<int>(line_idx,_num_chunk_sample);
  _curr_file_id++;
  
  random_shuffle(_samples.begin(), _samples.end());
  return _samples;
}
