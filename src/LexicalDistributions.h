#ifndef LEXICALDISTRIBUTION_H_
#define LEXICALDISTRIBUTION_H_

#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <utility>
#include <stack>
#include <set>
#include <cmath>
#include <map>
#include "boost/unordered_set.hpp"
#include "boost/unordered_map.hpp"
#include "boost/functional/hash.hpp"

#include "Types.h"
#include "Sentence.h"
#include "Data.h"
#include "Utilities.h"
#include "Writer.h"

using namespace std;
using namespace boost;

// typedef boost::unordered_map<long int, float> WordProbs;

class LexicalDistributions {
 public:
  WordProbs unigram_frequencies_of_source;
  WordProbs unigram_frequencies_of_target;

  LexProbs translation_table;

 public:
  LexicalDistributions(const WordProbs &lufs, const WordProbs &luft,
                       const LexProbs &t_t);

  WordProbs getUnigramFrequenciesOfSource();
  void setUnigramFrequenciesOfSource(const WordProbs &lus);

  WordProbs getUnigramFrequenciesOfTarget();
  void setUnigramFrequenciesOfTarget(const WordProbs &lut);

  LexProbs getTranslationTable() const;
  void setTranslationTable(const LexProbs &tt);

  // training of model-1 : fractional counts for word translation pairs in a
  // sentence that are not bigger than obtained by random choice are not added
  // to the count for that translation pair
  static float trainModel1(string &path_to_co_occurrence_map,
                           string &path_to_training_data,
                           string &path_to_t_table, int nbr_iterations);

  // renumbers t-table according to numbering maps from training data
  static void renumberTTable(string &path_to_source_token_numbering,
                             string &path_to_target_token_numbering,
                             string &path_to_t_table,
                             string &path_to_renumbered_table);

  // computes unigram frequencies of words for the whole corpus
  static void computeUnigramFrequencies(string &input_path_source,
                                        string &input_path_target,
                                        string &path_to_unigram_frequencies);

  // prepares a t-table for each document by filtering out words that are in the
  // document (document = numbered_data_stack)
  void prepareTTable(stack<Data> numbered_data, LexProbs &t_table);

  // prepares unigram frequencies for each document by filtering out words that
  // are in the document (document = numbered_data_stack)
  void prepareUnigramFrequencies(stack<Data> numbered_data,
                                 string &path_to_unigram_frequencies_of_source,
                                 string &path_to_unigram_frequencies_of_target);

  // loads the t-table into memory
  static LexProbs loadTTable(string &path_to_t_table);

  float getTTableEntry(const long int stok, const long int ttok) const;
  vector<float> getTTableEntries(const long int stok,
                                 const vector<long int> &ttoks) const;
};

#endif /*LEXICALDISTRIBUTIONS_H_*/
