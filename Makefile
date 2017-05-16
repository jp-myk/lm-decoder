#!/bin/make
CC = gcc
CFLAGS = -g -Wall
CXX = g++
CXXFLAGS = -g -Wall
LIBS=-L./3rdParty/lib  # install marisa trie to this path
INCLUDES=-I$(HOME)/local/include -I./util -I./3rdParty/include -I./src -I/
LDFLAGS = -lmarisa
OBJS = src/yomifuyoLM.o
TARGETS = train_pc train_svm decoder lmdecoder

.PHONY: all clean
.SUFFIXES: .c .cpp .o

all: $(TARGETS)

make_data: 
	wget -q -O sample_data/aozora.csv.gz  http://aozora-word.hahasoha.net/utf8/utf8_all.csv.gz
decoder: bin/decoder.o src/classifier/Model.o src/decoder/Feature.o src/decoder/Dic.o src/decoder/Lattice.o src/decoder/Decoder.o 
	$(CXX) $(CXXFLAGS)  -o bin/$@  $^ $(LIBS) $(LDFLAGS) $(INCLUDES)

lmdecoder: bin/lmdecoder.o src/ngram/SLM.o src/decoder/Dic.o src/classifier/Model.o  src/decoder/Feature.o src/decoder/Lattice.o src/decoder/Decoder.o src/decoder/NgramLMDecoder.o 
	$(CXX) $(CXXFLAGS)  -o bin/$@  $^ $(LIBS) $(LDFLAGS) $(INCLUDES)

train_pc: bin/train_pc.o src/utils/FileChunker.o src/classifier/Perceptron.o src/learner/ILearner.o src/learner/StructuredPerceptron.o src/classifier/Model.o src/decoder/Feature.o src/decoder/Dic.o src/decoder/Lattice.o src/decoder/Decoder.o 
	$(CXX) $(CXXFLAGS)  -o bin/train_pc  $^ $(LIBS) $(LDFLAGS) $(INCLUDES)


train_svm: bin/train_svm.o src/utils/FileChunker.o src/classifier/SVM.o src/learner/ILearner.o src/learner/StructuredSVM.o src/classifier/Model.o src/decoder/Feature.o src/decoder/Dic.o src/decoder/Lattice.o src/decoder/Decoder.o src/decoder/SVMDecoder.o 
	$(CXX) $(CXXFLAGS)  -o bin/train_svm  $^ $(LIBS) $(LDFLAGS) $(INCLUDES)


SLM_test: test/SLM_test.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(INCLUDES) -o test/$@  $^
	valgrind --error-limit=no --leak-check=full --show-reachable=yes -v ./test/SLM_test 

lattice_test: test/lattice_test.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(INCLUDES) -o test/$@  $^
	#valgrind --error-limit=no --leak-check=full --show-reachable=yes -v ./test/SLM_test 

file_chunk_test: test/file_chunk_test.o
	$(CXX) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(INCLUDES) -o test/$@  $^


clean:
	find ./src -name "*.o" | xargs -n 1 $(RM)  
	find ./bin -name "*.o" | xargs -n 1 $(RM)  
	$(RM) ./bin/decoder ./bin/lmdecoder ./bin/train_pc ./bin/train_svm


.c.o:
	$(CC) $(CFLAGS) -o $@ -c $^
.cpp.o:
	$(CXX) $(CXXFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -o $@ -c $^
.c:
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
.cpp:
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
