# Language Model Decoder
Decoding to transduce from sentence to both word and its context-dependent reading sequence.   
This repository is for my own study.

## key points
- Stochastic N-gram Language Model (ARPA Format)
- Linear Discriminative Model(Structured SVM/Perceptron)
- forward beam search / viterbi / backward a star
- extract n-best hypothesis
- use marisa trie to look up a dictionary( https://github.com/s-yata/marisa-trie )

## build
```shell
$ make decoder    # decoder with Linear Discriminative Model 
$ make lmdecoder  # decoder with N-gram Language Model
$ make train_pc   # train with Structured Perceptron
$ make train_svm  # train with Structured Support Vector Machine
```  

## run model training
- train linear discriminative model
```shell
$ ./bin/train_pc sample_data/sample.dic sample_data/sample.txt pc.model pc.dic
[DEBUG] src/utils/FileChunker.cpp:39:splitFile: file=0  sample=500
[DEBUG] src/utils/FileChunker.cpp:39:splitFile: file=1  sample=1000
[DEBUG] src/utils/FileChunker.cpp:39:splitFile: file=2  sample=1500
[DEBUG] src/utils/FileChunker.cpp:54:splitFile: file=3  sample=1508
iter=1  precision=0.725464
iter=2  precision=0.895225
iter=3  precision=0.960212
iter=4  precision=0.982759
iter=5  precision=0.996684
iter=6  precision=0.998674
iter=7  precision=0.999337
iter=8  precision=0.998674
iter=9  precision=0.999337
iter=10 precision=0.998674
[DEBUG] src/decoder/Dic.cpp:39:save: save dic=pc.dic
[DEBUG] src/classifier/Model.cpp:31:save: save model=pc.model
```

- train ngram lm    
Please use OSS LM Toolkit. SRI or IRSTLM or etc.    
if possibly, I will commit original source code later.

## run decoder
- lmdecoder
```shell
$ echo "平城京は奈良時代の日本の首都" | ./bin/lmdecoder sample_data/sample.dic sample_data/sample.3gram.arpa 
======== 1-BEST =========
平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -17.1093
======== N-BEST =========
1-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -17.1093
2-best  平城:ヒラジロ 京:ミヤコ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -19.542
3-best  平城:ヒラジロ 京:キョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -20.7536
4-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日:ニチ 本:ホン の:ノ 首都:シュト     -22.5837
5-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日:ヒ 本:ホン の:ノ 首都:シュト       -22.9183
```

- decoder
```shell
$ echo "平城京は奈良時代の日本の首都" | ./bin/decoder pc.dic pc.model
======== 1-BEST =========
平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       0.3
======== N-BEST =========
1-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       0.3000
2-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       0.3000
3-best  平城:ヒラジロ 京:ミヤコ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -0.2000
4-best  平城:ヒラジロ 京:ミヤコ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -0.2000
5-best  平城:ヒラジロ 京:キョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       -0.3000
```
