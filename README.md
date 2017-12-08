# Language Model Decoder
- Transducer from a sentence to word/reading sequence.   
- This repository is for my own study.

## key points
- Statistical N-gram Language Model (ARPA Format)
- Linear Discriminative Model(Structured SVM/Perceptron)
- Lattice search algorithm is simple forward viterbi / beam search / backward a star
- Support to extract n-best hypothesis
- Use marisa-trie for dictionary look-up( https://github.com/s-yata/marisa-trie )

## build
```shall
$ make
```

```shell
$ make decoder    # decoder with Linear Discriminative Model 
$ make lmdecoder  # decoder with N-gram Language Model
$ make train_pc   # train with Structured Perceptron
$ make train_svm  # train with Structured Support Vector Machine
```  

## run model training
- train linear discriminative model
```shell
$ ./bin/train_svm sample_data/sample.dic sample_data/sample.txt svm.model svm.dic
[INFO] src/utils/FileChunker.cpp:39:splitFile: file=0   sample=500
[INFO] src/utils/FileChunker.cpp:39:splitFile: file=1   sample=1000
[INFO] src/utils/FileChunker.cpp:39:splitFile: file=2   sample=1500
[INFO] src/utils/FileChunker.cpp:54:splitFile: file=3   sample=1508
iter=1  precision=0.306366
iter=2  precision=0.534483
iter=3  precision=0.784483
iter=4  precision=0.896552
iter=5  precision=0.930371
iter=6  precision=0.947613
iter=7  precision=0.976127
iter=8  precision=0.982759
iter=9  precision=0.988727
iter=10 precision=0.996684
[INFO] src/decoder/Dic.cpp:39:save: save dic=svm.dic
[INFO] src/classifier/Model.cpp:30:save: save model=svm.model
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
$ echo "平城京は奈良時代の日本の首都" | ./bin/decoder svm.dic svm.model 
======== 1-BEST =========
平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       5.0000
======== N-BEST =========
1-best  平城京:ヘイジョウキョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       5.0000
2-best  平城:ヒラジロ 京:キョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       4.9000
3-best  平城:ヒラジロ 京:ミヤコ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト       4.7000
4-best  平:タイラ 城:ジョウ 京:キョウ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト 4.6000
5-best  平:タイラ 城:ジョウ 京:ミヤコ は:ハ 奈良:ナラ 時代:ジダイ の:ノ 日本:ニッポン の:ノ 首都:シュト 4.5000

```
