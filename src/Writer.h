#ifndef WRITER_H_
#define WRITER_H_

#include <stack>
#include <utility>
#include <cmath>
#include <vector>
#include "boost/unordered_map.hpp"
#include "Sentence.h"
#include "Data.h"
#include "Cell.h"
#include "Chunk.h"
#include "LengthDistributions.h"
#include "LexicalDistributions.h"

using namespace std;
using namespace boost;

class Writer {
 public:
  // writes the output of the sentence aligner (i.e. the alignments) using
  // sentence ids
  static void writeAlignment(string filename, string dest_path, stack<Chunk> a);

  // writes the aligned sentences into files : one (untokenized) sentence per
  // line in 2 files (one for source language sentences and one for target
  // language sentences) an info file indicates the ids of 	the aligned
  // sentences
  static void writeAlignedData(const string path_to_source_untokenized_data,
                               const string path_to_target_untokenized_data,
                               const string path_to_alignment,
                               const string path_to_aligned_source_data,
                               const string path_to_aligned_target_data,
                               const string path_to_info_file);

  // if only one-to-one correspondences are needed
  // static void writeAlignedData_1_to_1(string path_to_source_untokenized_data,
  // string path_to_target_untokenized_data, string path_to_alignment, string
  // path_to_aligned_source_data, string path_to_aligned_target_data, string
  // path_to_info_file);

  // writes the token numbering maps into files
  static void writeTokenNumbering(
      map<long int, long int> &source_token_numbering,
      map<long int, long int> &target_token_numbering,
      string &path_to_source_token_numbering,
      string &path_to_target_token_numbering);

  // writes the training data into file
  static void writeTrainingData(const string path_to_training_data,
                                const string &filename, stack<Chunk> &a);

  // writes the Co-occurrence map into file
  static void writeCoOccurrenceMap(
      map<long int, vector<long int> > &co_occurrences,
      string &path_to_co_occurrence_map);

  // writes the t-table into file
  static void writeTTable(const vector<WordProbs> &t_table,
                          const string &path_to_t_table);

  // writes unigram frequencies into a file
  static void writeUnigramFrequencies(WordProbs unigram_freq_of_source,
                                      WordProbs unigram_freq_of_target,
                                      string &path_to_relative_frequencies);

  // for debugging : write prepared t-table into a file
  // static void writePreparedTTable(boost::unordered_map<long int,
  // WordProbs > &prepared_t_table, string
  // &path_to_prepared_t_table, string &paragraph_name);
};
#endif /*WRITER_H*/
