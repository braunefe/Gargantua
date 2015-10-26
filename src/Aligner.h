#ifndef ALIGN_H_
#define ALIGN_H_

#include <stack>
#include <list>
#include <vector>
#include <utility>
#include <cmath>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "Writer.h"
#include "Sentence.h"
#include "Data.h"
#include "Chunk.h"
#include "LengthAlignment.h"
#include "LengthDistributions.h"
#include "InitialAlignment.h"
#include "AlignmentClustering.h"
#include "boost/serialization/list.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/string.hpp"

using namespace std;

class Aligner {
  friend class boost::serialization::access;

 public:
  vector<list<pair<int, int> > > flat_diagonals;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &flat_diagonals;
  }

 public:
  Aligner(){};

  vector<list<pair<int, int> > > getFlatDiagonals();
  void setFlatDiagonals(vector<list<pair<int, int> > > diag);

  // makes sentence length alignment, returns correspondances with likelihood
  // higher than 0.05 and traceback
  pair<stack<Chunk>, vector<list<pair<int, int> > > > makeLengthBasedAlignment(
      stack<Data> &d, const LengthDistributions &l_d);

  // flattens the diagonal obtained by the initial (sentence-length) aligner
  // (all alignments are reduced to 0-to-1,1-to-1,1-to-0)
  list<pair<int, int> > flattenDiagonal(stack<pair<int, int> > &traceback);

  // make final alignment
  stack<Chunk> makeInitialAlignment(stack<Data> data,
                                    const LengthDistributions &l_d,
                                    const LexicalDistributions &lex_d,
                                    vector<list<pair<int, int> > > f_d);

  // make alignment clustering
  stack<Chunk> makeAlignmentClustering(stack<Chunk> sentence_alignment,
                                       LengthDistributions l_d,
                                       LexicalDistributions lex_d);
};

#endif /*ALIGN_H_*/
