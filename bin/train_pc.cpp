#include "learner/StructuredPerceptron.h"

int main(int argc, char *argv[]){
  char *dicfile = argv[1];
  char *train_corpus = argv[2];
  char *output_model = argv[3];  // if already exist, embedded training
  char *output_dic = argv[4];
  char *outdir = "tmp_dir";
  int iter_num = 10;
  int chunk_num = 500;

  StructuredPerceptron learner;
  learner.setDic(dicfile);
  learner.setModel(output_model);
  learner.learn(train_corpus, outdir, chunk_num, iter_num); // train
  learner.save(output_dic, output_model);   // save
  return 0;
}

