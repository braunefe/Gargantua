#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <utility>
#include "boost/regex.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/unordered_map.hpp"
#include "boost/filesystem.hpp"
#include "iostream"

#include "Types.h"
#include "Utilities.h"
#include "Reader.h"
#include "Sentence.h"
#include "Data.h"
#include "LengthDistributions.h"
#include "LexicalDistributions.h"
#include "Serializers.h"
#include "Aligner.h"
#include "Writer.h"

using namespace std;
namespace ba = boost::archive;
namespace bfs = boost::filesystem;

int main() {
  // 1. Prepare corpus : lowercase data and transform into integer
  // representation
  cout << "Preparing corpus..." << endl;

  // make directory for data to align
  bfs::create_directory("../input_documents/");

  // path to untokenized data files
  string path_source_untok =
      "../corpus_to_align/source_language_corpus_untokenized/";
  string path_target_untok =
      "../corpus_to_align/target_language_corpus_untokenized/";

  // string path_europarl_source_tok =
  // "../EuroparlFr_untagged_tokenized_lowercased/";
  string path_source_tok =
      "../corpus_to_align/source_language_corpus_tokenized/";
  string path_target_tok =
      "../corpus_to_align/target_language_corpus_tokenized/";

  // path to tagged prepared data (tokenized, lowercased)
  string path_source_prep =
      "../corpus_to_align/source_language_corpus_prepared/";
  string path_target_prep =
      "../corpus_to_align/target_language_corpus_prepared/";

  // Make destination for tagged untokkenized data
  bfs::create_directory(
      "../input_documents/source_language_corpus_untokenized_ids/");
  bfs::create_directory(
      "../input_documents/target_language_corpus_untokenized_ids/");

  // path to tagged untokenized data
  string dest_source_untok =
      "../input_documents/source_language_corpus_untokenized_ids/";
  string dest_target_untok =
      "../input_documents/target_language_corpus_untokenized_ids/";

  // Tagg untokenized Europarl files
  Reader::makeIdTaggs(path_source_untok, path_target_untok, dest_source_untok,
                      dest_target_untok);

  cout << "Untokenized data tagged !" << endl;

  // Make destination for tagged untokkenized data
  bfs::create_directory(
      "../input_documents/source_language_prepared_data_ids/");
  bfs::create_directory(
      "../input_documents/target_language_prepared_data_ids/");

  // path to tagged prepared data
  string dest_source_prep =
      "../input_documents/source_language_prepared_data_ids/";
  string dest_target_prep =
      "../input_documents/target_language_prepared_data_ids/";

  // Tagg tokenized Europarl files
  Reader::makeIdTaggs(path_source_prep, path_target_prep, dest_source_prep,
                      dest_target_prep);

  cout << "Lowercased data tagged !" << endl;

  // Make destination for tagged data with words represented as ints
  bfs::create_directory("../input_documents/source_language_corpus_numbered/");
  bfs::create_directory("../input_documents/target_language_corpus_numbered/");

  // destination for numbered data
  string dest_source = "../input_documents/source_language_corpus_numbered/";
  string dest_target = "../input_documents/target_language_corpus_numbered/";

  // Number tokenized Europarl files
  Reader::numberData(dest_source_prep, dest_target_prep, dest_source,
                     dest_target);

  cout << "Input data numbered !" << endl;

  // remove non-numbered files
  Utilities::removeDirectory(
      "../input_documents/source_language_prepared_data_ids/");
  Utilities::removeDirectory(
      "../input_documents/target_language_prepared_data_ids/");

  // 2. Make sentence-length based alignment, extract training data for model-1
  // and flat diagonals

  // make directories for training data
  bfs::create_directory("./training_data/");
  bfs::create_directory("./training_data/not_pruned/");

  // make directory for flat diagonal from sentence alignment
  bfs::create_directory("./diagonals/");

  // paths to training data and diagonals
  const string training_data = "./training_data/not_pruned/";
  const string serialized_diagonals = "./diagonals/";

  stack<string> filenames;

  cout << "getting files... " << endl;
  filenames = Utilities::findAllFiles(dest_source);

  // Storage of raw relative frequencies of sentence lengths
  map<int, float> relative_frequency_of_source_length;  // necessary for
                                                        // construction of
                                                        // frequency object
  map<int, float> relative_frequency_of_target_length;

  // Construct parameters for sentence-length based alignment
  LengthDistributions my_length_distributions =
      LengthDistributions(0.0, relative_frequency_of_source_length,
                          relative_frequency_of_target_length);
  cout << "Estimating mean of Poisson..." << endl;

  // estimation of Poissson distribution mean
  my_length_distributions.estimateParamOfPoisson(dest_source, dest_target);

  cout << "Computing raw realtive frequencies... ";
  // estimation of raw relative frequencies of source and target sentence length
  my_length_distributions.computeRawRelativeFrequencies(dest_source,
                                                        dest_target);
  cout << "done. " << endl;

  // Archivate estimated parameters and distributions
  Serializers::serializeLengthDistributions(my_length_distributions);

  // Restore estimated parameters and distributions
  LengthDistributions my_restored_length_distributions =
      Serializers::restoreLengthDistributions();

  // make data structure to store flat diagonal for a document
  list<pair<int, int> > flat_diagonal;

  // go through filenames and make length alignment
  auto fn_vector = Utilities::stackToVector(filenames);
  int n_files = static_cast<int>(fn_vector.size());
  cout << "Processing " << n_files << " files" << endl;

#pragma omp parallel for schedule(dynamic)
  for (int file_idx = 0; file_idx < n_files; ++file_idx) {
    const string filename = fn_vector[file_idx];
    cout << "[1/2] Processing " << file_idx << " / " << n_files
         << " Filename : " << filename << endl;

    // split europal-like file into paragraphs. In case of wrong tagging
    // or absence of tags, the whole document is aligned
    stack<Data> paragraphs = Reader::splitEuroparlDocumentIntoParagraphs(
        Utilities::findEuroparlFile(dest_source, filename).c_str(),
        Utilities::findEuroparlFile(dest_target, filename).c_str());

    cout << "Making sentence length based alignment..." << endl;

    // Sentence align stack of paragraphs
    Aligner my_aligner = Aligner();
    pair<stack<Chunk>, vector<list<pair<int, int> > > > best_ones_and_diagonal =
        my_aligner.makeLengthBasedAlignment(paragraphs,
                                            my_restored_length_distributions);
    cout << "Alignment done." << endl;

    stack<Chunk>& all_best_ones = best_ones_and_diagonal.first;
    vector<list<pair<int, int> > >& all_diagonals =
        best_ones_and_diagonal.second;

    // Serialize flat diagonals
    Aligner my_aligner_for_diagonal = Aligner();
    my_aligner_for_diagonal.setFlatDiagonals(all_diagonals);
    Serializers::serializeFlatDiagonals(my_aligner_for_diagonal,
                                        serialized_diagonals, filename);

    cout << "Writing training data." << endl;
    Writer::writeTrainingData(training_data, filename, all_best_ones);
  }

  // 3. Remove rare occurrences from data and training data, number training
  // data, train model-1 and unigram frequencies.

  // Remove rare occurrences from training data
  // make directories for pruned training data
  bfs::create_directory("./training_data/pruned/");

  string pruned_training_data = "./training_data/pruned/";

  // prune training data
  int min_occurrences_for_token =
      3;  // all token with occurrences < 3 are filtered out

  cout << "Filtering out rare words from training data ..." << endl;
  pair<vector<long int>, vector<long int> > rare_words =
      Reader::filterOutRareWordsFromTrainingData(
          min_occurrences_for_token, training_data, pruned_training_data);

  // remove non-pruned training data
  Utilities::removeDirectory("./training_data/not_pruned/");

  // Remove rare occurrences from data
  // make directories for pruned data
  bfs::create_directory("../input_documents/filtered_source_data/");
  bfs::create_directory("../input_documents/filtered_target_data/");

  string filtered_source = "../input_documents/filtered_source_data/";
  string filtered_target = "../input_documents/filtered_target_data/";

  cout << "Filtering out rare words from data ..." << endl;

  Reader::filterOutRareWordsFromData(rare_words.first, rare_words.second,
                                     dest_source, dest_target, filtered_source,
                                     filtered_target);

  // remove non-pruned data
  Utilities::removeDirectory(
      "../input_documents/source_language_corpus_numbered/");
  Utilities::removeDirectory(
      "../input_documents/target_language_corpus_numbered/");

  // Number training data
  // make directory for numbered training data
  bfs::create_directory("./training_data/numbered/");

  string numbered_training_data = "./training_data/numbered/";

  cout << "Numbering training data ..." << endl;
  Reader::numberTrainingData(pruned_training_data, numbered_training_data);

  // remove non-numbered data
  Utilities::removeDirectory("./training_data/pruned/");

  // paths to numbering maps
  string source_token_numbering = "./numbering_maps/source_token_numbering.txt";
  string target_token_numbering = "./numbering_maps/target_token_numbering.txt";

  // Make co-occurrence map
  // path to co-occurrence map
  string co_occurrence_map = "./training_data/co_occurrence_map.txt";

  // make co-occurrence map
  cout << "Making co-occurrence map... " << endl;
  Reader::makeCoOccurrenceMap(numbered_training_data, co_occurrence_map);

  // Train model-1 and unigram frequency model
  // make directory for lexical distributions
  bfs::create_directory("./lexical_distributions/");

  // path to pruned_t_table
  string pruned_t_table = "./lexical_distributions/pruned_t_table.txt";
  string renumbered_table = "./lexical_distributions/renumbered_t_table.txt";

  cout << "training model-1" << endl;

  // train model-1
  LexicalDistributions::trainModel1(co_occurrence_map, numbered_training_data,
                                    pruned_t_table, 4);

  // renumber t-table
  cout << "renumbering t-table..." << endl;
  LexicalDistributions::renumberTTable(source_token_numbering,
                                       target_token_numbering, pruned_t_table,
                                       renumbered_table);

  // make directory for unigram language model
  bfs::create_directory("./lexical_distributions/unigram_language_model/");

  // path to unigram language model
  string unigram_frequencies =
      "./lexical_distributions/unigram_language_model/";

  // compute local unigram frequencies
  cout << "computing unigram frequencies ..." << endl;
  LexicalDistributions::computeUnigramFrequencies(
      filtered_source, filtered_target, unigram_frequencies);

  // 4. Make initial alignment and clustering
  // make directory for alignments
  bfs::create_directory("./alignments/");
  string alignments = "./alignments/";

  // make directory for prepared t-tables
  bfs::create_directory("./lexical_distributions/prepared_t_tables/");

  string prepared_tables = "./lexical_distributions/prepared_t_tables/";

  // path to files containing the unigram language model
  string unigram_frequencies_of_source =
      "./lexical_distributions/unigram_language_model/"
      "Unigram_frequencies_of_source.txt";
  string unigram_frequencies_of_target =
      "./lexical_distributions/unigram_language_model/"
      "Unigram_frequencies_of_target.txt";

  // Make objects in fields of lexical distributions
  WordProbs local_unigram_frequencies_of_source;
  WordProbs local_unigram_frequencies_of_target;
  LexProbs translation_table;

  // construct lexical distribution
  LexicalDistributions my_lexical_distributions = LexicalDistributions(
      local_unigram_frequencies_of_source, local_unigram_frequencies_of_target,
      translation_table);

  // prepare t-table for document
  cout << "Loading t-table..." << endl;
  translation_table = LexicalDistributions::loadTTable(renumbered_table);

  // output alignment
  stack<Chunk> alignment;

  cout << "getting files... " << endl;
  filenames = Utilities::findAllFiles(filtered_source);

  // go through filenames and make Final alignment
  // This is slow!
  fn_vector = Utilities::stackToVector(filenames);
  n_files = static_cast<int>(fn_vector.size());

#pragma omp parallel for schedule(dynamic)
  for (int file_idx = 0; file_idx < n_files; ++file_idx) {
    const string filename = fn_vector[file_idx];
    cout << "[2/2] Processing " << file_idx << " / " << n_files
         << " Filename : " << filename << endl;

    stack<Data> paragraphs = Reader::splitEuroparlDocumentIntoParagraphs(
        Utilities::findEuroparlFile(filtered_source, filename).c_str(),
        Utilities::findEuroparlFile(filtered_target, filename).c_str());
    const vector<Data> paragraph_vec = Utilities::stackToVector(paragraphs);

    // restore the flat diagonals computed from the initial alignment
    vector<list<pair<int, int> > > flat_diagonals =
        Serializers::restoreFlatDiagonals(serialized_diagonals, filename)
            .getFlatDiagonals();

    // Restore length distributions
    cout << "Restoring length distributions... " << endl;
    LengthDistributions my_restored_length_distributions =
        Serializers::restoreLengthDistributions();

    cout << "Preparing t-table..." << endl;
    // construct lexical distribution
    LexicalDistributions local_lexical_distributions = LexicalDistributions(
        local_unigram_frequencies_of_source,
        local_unigram_frequencies_of_target, translation_table);
    local_lexical_distributions.prepareTTable(paragraph_vec, translation_table);

    cout << "Preparing unigram frequencies..." << endl;
    local_lexical_distributions.prepareUnigramFrequencies(
        paragraph_vec, unigram_frequencies_of_source,
        unigram_frequencies_of_target);

    cout << "Making initial alignment... " << endl;  // this is the slowest step
    Aligner myAligner = Aligner();
    alignment = myAligner.makeInitialAlignment(
        paragraph_vec, my_restored_length_distributions,
        local_lexical_distributions, flat_diagonals);

    cout << "Making alignment clustering... " << endl;
    alignment = myAligner.makeAlignmentClustering(
        alignment, my_restored_length_distributions,
        local_lexical_distributions);

// write alignments using sentence ids
#pragma omp critical
    Writer::writeAlignment(filename, alignments, alignment);
  }  // end of while

  // write aligned data : aligned source sentences /aligned target sentences
  // /info file
  bfs::create_directory("./output_data_aligned/");
  string aligned_source_data =
      "./output_data_aligned/aligned_sentences_source_language.txt";
  string aligned_target_data =
      "./output_data_aligned/aligned_sentences_target_language.txt";
  string info_file = "./output_data_aligned/info.txt";

  Writer::writeAlignedData(dest_source_untok, dest_target_untok, alignments,
                           aligned_source_data, aligned_target_data, info_file);

  return 0;
}
