#ifndef InitialAlignment_H_
#define InitialAlignment_H_

#include <stack>
#include <utility>
#include <cmath>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "boost/unordered_map.hpp"
#include "Sentence.h"
#include "Data.h"
#include "Cell.h"
#include "Chunk.h"
#include "LengthDistributions.h"
#include "LexicalDistributions.h"

class InitialAlignment {
 private:
  stack<Chunk> alignment;

  bool expand_band_1;  // band expansion information
  bool expand_band_2;

  bool dynamic_programming_done;  // true when DP procedure is done

 protected:
  list<pair<int, int> > diagonal;  // diagonal from initial alignment

 public:
  InitialAlignment(const list<pair<int, int> > &f_d);

  list<pair<int, int> > getDiagonal() const;
  void setDiagonal(const list<pair<int, int> > &f_d);

  stack<Chunk> getAlignment();
  void setAlignment(stack<Chunk>);

  bool getExpandBand1() const;
  void setExpandBand1(bool);

  bool getExpandBand2() const;
  void setExpandBand2(bool);

  bool getReduceBand1() const;
  void setReduceBand1(bool);

  //  bool getReduceBand2() const;
  //  void setReduceBand2(bool);

  bool getDynamicPrgrammingState() const;
  void setDynamicProgrammingState(bool);

  // sentence length and lexical based distance function as defined in Moore
  // 2002 but restricted to 1-to-0/0-to-1 and 1-to-1 alignments and with Viterbi
  // model 1 instead of summing up over 		all alignments
  float distanceFunctionMoore(const LengthDistributions &length_distributions,
                              const LexicalDistributions &lexical_distributions,
                              vector<long int> &st1, vector<long int> &tt1);

  // align : reiterate dynamic programming with incrementing bandwidth until the
  // whole text has been aligned
  void align(Data &d, const LengthDistributions &length_distributions,
             const LexicalDistributions &lexical_distributions);

  // dynamic programming : makes an approximate alignment on a narrow band
  // around the flat diagonal extracted from the length-based alignment. If the
  // alignment comes close to the bounds 		reiterate and replace the
  // flat
  // diagonal by the approximate alignment.
  void dynamicProgramming(Data &d,
                          const LengthDistributions &length_distributions,
                          const LexicalDistributions &lexical_distributions,
                          int bandwidth_1, int bandwidth_2);
};

#endif /*InitialAlignment_H_*/
