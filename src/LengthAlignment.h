#ifndef LENGTHALIGNMENT_H_
#define LENGTHALIGNMENT_H_

#include <stack>
#include <utility>
#include <cmath>
#include <vector>
#include "Sentence.h"
#include "Data.h"
#include "Cell.h"
#include "Chunk.h"
#include "LengthDistributions.h"

/*
Length-based sentence aligner: makes length-based alignment as in Moore or a
modified version
*/

class LengthAlignment {
 private:
  stack<pair<int, int> > trace_back;
  stack<Chunk> alignment;

 protected:
  //	int bandwidth;

 public:
  LengthAlignment(){};

  stack<pair<int, int> > getTraceBack();
  void setTraceBack(stack<pair<int, int> > t_b);

  stack<Chunk> getAlignment();
  void setAlignment(stack<Chunk>);

  // sentence length based distance function as defined in Moore 2002
  float distanceFunctionMoore(const LengthDistributions &l_d, int x1, int x2,
                              int y1, int y2);

  // dynamic Programming procedure : searches a barrow band around the main
  // diagonal (i=j). If the search comes close to the bound, the procedure
  // reiterates
  void dynamicProgramming(const Data &d, const LengthDistributions &l_d,
                          int bandwith = 0);

  // filters out the best 1-to-1 correspondences from the sentence length based
  // alignment
  stack<Chunk> filterOutBestOneToOnes(const LengthDistributions &l_d,
                                      stack<Chunk> a);
};
#endif /*LENGTH_ALIGNMENT_H*/
