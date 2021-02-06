#include "learner/StructuredSVM.h"

int main(int argc, char *argv[]){
  char *dicfile = argv[1];
  char *train_corpus = argv[2];
  char *output_model = argv[3];  // if already exist, embedded training
  char *output_dic = argv[4];
  const char *outdir = "tmp_dir";
  int iter_num = 10;
  int chunk_num = 500;

  StructuredSVM learner;  
  if (learner.setDic(dicfile) != 0) {
    std::cerr << "failed setDict:" << dicfile << std::endl;
    return 1;
  }
  if (learner.setModel(output_model) != 0) {
    std::cerr << "failed setModel:" << output_model << std::endl;
    return 1;
  }
  learner.learn(train_corpus, outdir, chunk_num, iter_num); // train
  // save
  if (learner.save(output_dic, output_model) != 0) {
    std::cerr << "failed save:" << output_dic << "," << output_model << std::endl;
    return 1;
  }
  return 0;
}
