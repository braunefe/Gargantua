#ifndef ALIGNMENTCLUSTERING_H_
#define ALIGNMENTCLUSTERING_H_

#include <stack>
#include <map>
#include <utility>
#include <cmath>
#include <vector>
#include "Sentence.h"
#include "Data.h"
#include "Chunk.h"
#include "LengthDistributions.h"
#include "LexicalDistributions.h"

using namespace std;

class AlignmentClustering {
 private:
  stack<Chunk> sentence_alignment;
  stack<Chunk> expanded_alignment;

  // prior distributions of alignment types
  float prior10_01;
  float prior_11;
  float prior12_21;
  float prior13_31;
  float prior14_41;

 public:
  AlignmentClustering(stack<Chunk> sentence_alignment);

  stack<Chunk> getSentenceAlignment();
  void setSentenceAlignment(stack<Chunk>);

  stack<Chunk> getExpandedAlignment();
  void setExpandedAlignment(stack<Chunk>);

  // compute bead priors (P_ak()) and lambda factor for successions of
  // insertions and deletions
  void estimatePriors(stack<Chunk> a);

  // distance function for making merging decisions (based on distance function
  // as defined in Moore 2002 but with Viterbi model 1 instead of summing up
  // over all alignments)
  long double mergeDistanceFunction(LengthDistributions &l_d,
                                    LexicalDistributions &lex_d,
                                    vector<long int> st, vector<long int> tt,
                                    vector<int> ss, vector<int> ts);

  void alignmentClustering(stack<Chunk> sentence_alignment,
                           LengthDistributions &l_d,
                           LexicalDistributions &lex_d);
};

#endif /*ALIGNMENTCLUSTERING_H_*/
