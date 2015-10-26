#include "Writer.h"
#include "LexicalDistributions.h"

// TO DO : write all data, including 1-to-1 correspondences
/*void Writer::writeAlignedData(string path_to_source_untokenized_data, string
path_to_target_untokenized_data, string path_to_alignment, string
path_to_aligned_source_data, string path_to_aligned_target_data, string
path_to_info_file)
{
        cout << " ";

        //get files in directory
        stack<string> filenames;

        //filenames = Utilities::findAllFiles(path_to_alignment);
        filenames.push("ep-97-07-17");

        //make output stream
        //file containing source sentences
        ofstream fout1(path_to_aligned_source_data.c_str());
        //file containing target sentences
        ofstream fout2(path_to_aligned_target_data.c_str());
        //info file
        ofstream fout3(path_to_info_file.c_str());

        while(!filenames.empty())
        {
                string filename = filenames.top();
                filenames.pop();

                //read untokenized data
                string file_name = filename; //store filename for .c_str
transformation

                Data my_data =
Reader::readEuroparlDocument(Utilities::findEuroparlFile(path_to_source_untokenized_data,file_name).c_str(),Utilities::findEuroparlFile(path_to_target_untokenized_data,file_name).c_str());

                //path to numbered training data
                string path = path_to_alignment;

                //open file
                ifstream fin1(path.append(filename).c_str());

                //separator between source and target token
                string separator = ";";

                //write filename into info file
                //cout << "File : ";

                //cout << ". ";

                //store input line
                string line;

                //read alignments and find corresponding sentences
                while( getline ( fin1,line ) )
                {
                        char * sentCstr;
                        char * t;
                        sentCstr = new char [line.size()+1];
                        strcpy (sentCstr, line.c_str());
                        t=strtok(sentCstr," ");

                        vector<long int> source_sentence_ids;
                        vector<long int> target_sentence_ids;

                        while( t!= separator)
                        {
                                source_sentence_ids.push_back(atol(t));
                                t=strtok_r(NULL," ", &sp);
                        }

                        if( t == separator) // ignore separator
                        {
                                t=strtok_r(NULL," ", &sp);
                        }

                        while( t != NULL)
                        {
                                target_sentence_ids.push_back(atol(t));
                                t=strtok_r(NULL," ", &sp);
                        }

                        //write source sentence ids into info file
                        fout3 << "Source Sentence(s) : ";

                        //iterate through source and target sentence ids
                        vector<long int> :: iterator itr_sent_ids;

                        //look for untokenized source sentences and write into
file
                        for(itr_sent_ids = source_sentence_ids.begin();
itr_sent_ids != source_sentence_ids.end(); itr_sent_ids++)
                        {
                                long int sentence_id = *itr_sent_ids;

                                //write each source sentence id into info file
                                fout3 << sentence_id << " ";

                                //iterate through data and find sentences
corresponding to ids
                                bool is_found = 0; //control variable for while
loop

                                vector<Sentence> :: iterator itr_sentences =
my_data.getSourceSentences().begin();

                                while(is_found == 0 && itr_sentences !=
my_data.getSourceSentences().end())
                                {
                                        Sentence sent = *itr_sentences;
                                        if(sentence_id == sent.getPosition())
                                        {
                                                vector<string> token =
sent.getToken();
                                                vector<string> :: iterator
itr_tok;
                                                for(itr_tok = token.begin();
itr_tok != token.end(); itr_tok++)
                                                {
                                                        string tok = *itr_tok;
                                                        fout1 << tok << " ";
                                                }
                                        is_found = 1;
                                        }
                                        itr_sentences++;
                                }
                        }
                        fout3 << ". ";
                        fout1 << endl;

                        //write target sentence ids into info file
                        fout3 << "Target Sentence(s) : ";
                        //look for untokenized target sentences and write into
file
                        for(itr_sent_ids = target_sentence_ids.begin();
itr_sent_ids != target_sentence_ids.end(); itr_sent_ids++)
                        {
                                long int sentence_id = *itr_sent_ids;
                                //write each source sentence id into info file
                                fout3 << sentence_id << " ";

                                //iterate through data and find sentences
corresponding to ids
                                bool is_found = 0; //control variable for while
loop

                                vector<Sentence> :: iterator itr_sentences =
my_data.getTargetSentences().begin();

                                while(is_found == 0 && itr_sentences !=
my_data.getTargetSentences().end())
                                {
                                        Sentence sent = *itr_sentences;
                                        if(sentence_id == sent.getPosition())
                                        {
                                                vector<string> token =
sent.getToken();
                                                vector<string> :: iterator
itr_tok;
                                                for(itr_tok = token.begin();
itr_tok != token.end(); itr_tok++)
                                                {
                                                        string tok = *itr_tok;
                                                        fout2 << tok << " ";
                                                }
                                        is_found = 1;
                                        }
                                        itr_sentences++;
                                }
                        }
                        fout3 << ". " << endl;
                        fout2 << endl;
                        delete [] sentCstr;
                        delete [] t;
                        line.clear();
                        source_sentence_ids.clear();
                        target_sentence_ids.clear();
                }
                fin1.close();
        }//end of while through files
}*/

// writes the output of the sentence aligner (i.e. the alignments) using
// sentence ids
void Writer::writeAlignment(string filename, string dest_path,
                            stack<Chunk> alignment) {
  // Make path
  string combined = dest_path.append(filename.append(".txt"));

  // If alignment is empty : signal it
  if (alignment.empty()) {
    cout << "File : " << combined
         << " is empty or malformed. No alignment has been found." << endl;
  } else {
    // file containing the printed positions
    ofstream fout(combined.c_str());

    stack<Chunk> a;

    // reverse stack
    while (!alignment.empty()) {
      Chunk my_chunk = alignment.top();
      a.push(my_chunk);
      alignment.pop();
    }

    while (!a.empty()) {
      Chunk myChunk = a.top();

      // extract chunk info and write into file
      vector<long int> sc = myChunk.getSourceChunkPosition();
      vector<long int> tc = myChunk.getTargetChunkPosition();

      vector<long int>::iterator itrs;
      vector<long int>::iterator itrt;

      for (itrs = sc.begin(); itrs != sc.end(); itrs++) {
        long int sp = *itrs;
        fout << sp << " ";
      }

      fout << "; ";

      for (itrt = tc.begin(); itrt != tc.end(); itrt++) {
        long int tp = *itrt;
        fout << tp << " ";
      }

      fout << endl;
      a.pop();
    }  // end of while
    // close fout
    fout.close();
  }  // end of else
}

// writes the aligned sentences into files : one sentence per line in 2 files
// (one for source language sentences and one for target language sentences) an
// info file indicates the ids of 	the aligned sentences
void Writer::writeAlignedData(const string path_to_source_untokenized_data,
                              const string path_to_target_untokenized_data,
                              const string path_to_alignment,
                              const string path_to_aligned_source_data,
                              const string path_to_aligned_target_data,
                              const string path_to_info_file) {
  // get files in directory
  stack<string> filenames;

  filenames = Utilities::findAllFiles(path_to_alignment);

  // make output stream
  // file containing source sentences
  ofstream fout1(path_to_aligned_source_data.c_str());
  cout << "aligned source data in : " << path_to_aligned_source_data << endl;
  // file containing target sentences
  ofstream fout2(path_to_aligned_target_data.c_str());
  cout << "aligned target data in : " << path_to_aligned_target_data << endl;
  // info file
  ofstream fout3(path_to_info_file.c_str());

  while (!filenames.empty()) {
    string filename = filenames.top();

    cout << "Processing file " << filename << endl;

    filenames.pop();

    Data my_data = Reader::readEuroparlDocument(
        Utilities::findEuroparlFile(path_to_source_untokenized_data, filename)
            .c_str(),
        Utilities::findEuroparlFile(path_to_target_untokenized_data, filename)
            .c_str());

    // make hash map with data
    boost::unordered_map<long int, vector<string> > source_sentences_map;
    boost::unordered_map<long int, vector<string> > target_sentences_map;

    vector<Sentence> source_sentences = my_data.getSourceSentences();
    vector<Sentence> target_sentences = my_data.getTargetSentences();

    for (const auto &sent : target_sentences) {
      // ignore insertions and deletions
      if (sent.getPosition() == -1) {
    	  cout << "Skipping source " << sent.getPosition() << endl;
      } else {
        pair<long int, vector<string> > source_entry(sent.getPosition(),
                                                     sent.getToken());
        source_sentences_map.insert(source_entry);
      }
    }

    for (const auto &sent : target_sentences) {
      // ignore insertions and deletions
      if (sent.getPosition() == -1) {
    	  cout << "Skipping target " << sent.getPosition() << endl;
      } else {
        pair<long int, vector<string> > target_entry(sent.getPosition(),
                                                     sent.getToken());
        target_sentences_map.insert(target_entry);
      }
    }

    // for debugging
    // cout << "Size of source sentences " << source_sentences_map.size() <<
    // endl;
    // cout << "Size of target sentences " << target_sentences_map.size() <<
    // endl;

    // path to aligned files
    string path = path_to_alignment;

    // open file
    string combined = path.append((filename).append(".txt"));
    ifstream fin1(combined.c_str());
    if (!fin1) {
      cout << "Path to alignments not found" << combined << endl;
    } else {
      // separator between source and target token
      string separator = ";";

      // store input line
      string line;

      // read sentence ids in alignments and find corresponding sentences
      while (getline(fin1, line)) {
        // tokenize
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        vector<long int> source_sentence_ids;
        vector<long int> target_sentence_ids;

        while (t != separator) {
          source_sentence_ids.push_back(atol(t));
          t = strtok_r(NULL, " ", &sp);
        }

        if (t == separator)  // ignore separator
        {
          t = strtok_r(NULL, " ", &sp);
        }

        while (t != NULL) {
          target_sentence_ids.push_back(atol(t));
          t = strtok_r(NULL, " ", &sp);
        }

        // iterate through source and target sentence ids
        vector<long int>::iterator itr_sent_ids;

        // indicates if a sentence has been inserted or deleted
        bool insertion_deletion = 0;

        // ignore insertions or deletions
        if (source_sentence_ids.front() == -1 ||
            target_sentence_ids.front() == -1) {
          insertion_deletion = 1;
        } else {
          // write filename into info file
          fout3 << filename << "\t";

          // look for untokenized source sentences and write into file
          for (itr_sent_ids = source_sentence_ids.begin();
               itr_sent_ids != source_sentence_ids.end(); itr_sent_ids++) {
            long int sentence_id = *itr_sent_ids;

            // write each source sentence id into info file
            if (itr_sent_ids == source_sentence_ids.end() - 1) {
              fout3 << sentence_id;
            } else {
              fout3 << sentence_id << " ";
            }

            const auto stoks = source_sentences_map.find(sentence_id);
            if (stoks == source_sentences_map.end()) {
              cout << "Could not find source sentence_id " << sentence_id << endl;
//              abort();
            } else {
				for (const auto& tok : stoks->second) {
				  fout1 << tok << " ";
				}
            }
          }
          insertion_deletion = 0;
          fout3 << "\t";
        }
        if (insertion_deletion == 0) {
          fout1 << endl;
        }

        // ignore insertions and deletions
        if (source_sentence_ids.front() == -1 ||
            target_sentence_ids.front() == -1) {
          insertion_deletion = 1;
        } else {
          // write target sentence ids into info file
          // look for untokenized target sentences and write into file
          for (itr_sent_ids = target_sentence_ids.begin();
               itr_sent_ids != target_sentence_ids.end(); itr_sent_ids++) {
            long int sentence_id = *itr_sent_ids;
            // write each source sentence id into info file
            if (itr_sent_ids == target_sentence_ids.end() - 1) {
              fout3 << sentence_id;
            } else {
              fout3 << sentence_id << " ";
            }

            const auto ttoks = target_sentences_map.find(sentence_id);
            if (ttoks == target_sentences_map.end()) {
              cout << "Could not find sentence_id " << sentence_id << endl;
//              abort();
            } else {
            	for (const auto& tok : ttoks->second) {
                    fout2 << tok << " ";
            	}
            }
          }
          insertion_deletion = 0;
        }
        if (insertion_deletion == 0) {
          fout2 << endl;
          fout3 << endl;
        }
        source_sentence_ids.clear();
        target_sentence_ids.clear();
        delete[] sentCstr;
        delete[] t;
        line.clear();
      }
    }
    // close fin1
    fin1.close();
  }  // end of else
  // close fouts
  fout1.close();
  fout2.close();
  fout3.close();
}

// writes the token numbering maps into files
void Writer::writeTokenNumbering(
    map<long int, long int> &source_token_numbering,
    map<long int, long int> &target_token_numbering,
    string &path_to_source_token_numbering,
    string &path_to_target_token_numbering) {
  // make output files
  ofstream fout1(path_to_source_token_numbering.c_str());
  ofstream fout2(path_to_target_token_numbering.c_str());

  map<long int, long int>::iterator itr_token_numbering;

  // write source token numbering
  for (itr_token_numbering = source_token_numbering.begin();
       itr_token_numbering != source_token_numbering.end();
       itr_token_numbering++) {
    pair<long int, long int> p = *itr_token_numbering;
    fout1 << p.first << " : " << p.second << endl;
  }

  // write source token numbering
  for (itr_token_numbering = target_token_numbering.begin();
       itr_token_numbering != target_token_numbering.end();
       itr_token_numbering++) {
    pair<long int, long int> p = *itr_token_numbering;
    fout2 << p.first << " : " << p.second << endl;
  }

  fout1.close();
  fout2.close();
}

// writes the training data into file
void Writer::writeTrainingData(const string path_to_training_data,
                               const string &filename, stack<Chunk> &a) {
  // 1. Make path
  const string filename_with_path =
      path_to_training_data + filename + string(".txt");
  //  filename.clear();

  cout << "filename_with_path " << filename_with_path << endl;

  // file containing the printed positions
  ofstream fout(filename_with_path.c_str());

  // string for null word : to be appended at the end of each source chunk
  string null_word = "NULL_WORD";

  while (!a.empty()) {
    Chunk myChunks = a.top();

    // get chunnk information
    vector<vector<long int> > source_chunks = myChunks.getSourceChunkToken();
    vector<vector<long int> > target_chunks = myChunks.getTargetChunkToken();

    vector<vector<long int> >::iterator itr_chunks;

    // iterate through source chunk and write source token into file
    for (itr_chunks = source_chunks.begin(); itr_chunks != source_chunks.end();
         itr_chunks++) {
      vector<long int> source_chunk = *itr_chunks;
      vector<long int>::iterator itr_sentences;

      for (itr_sentences = source_chunk.begin();
           itr_sentences != source_chunk.end(); itr_sentences++) {
        long int token = *itr_sentences;
        fout << token << " ";
      }
    }
    fout << null_word << endl;

    // iterate through target chunk and write target token into file
    for (itr_chunks = target_chunks.begin(); itr_chunks != target_chunks.end();
         itr_chunks++) {
      vector<long int> target_chunk = *itr_chunks;
      vector<long int>::iterator itr_sentences;

      for (itr_sentences = target_chunk.begin();
           itr_sentences != target_chunk.end(); itr_sentences++) {
        long int token = *itr_sentences;
        fout << token << " ";
      }
    }
    a.pop();
    fout << endl;
  }
  // close fout
  fout.close();
}

// writes the Co-occurrence map into file
void Writer::writeCoOccurrenceMap(
    map<long int, vector<long int> > &co_occurrences,
    string &path_to_co_occurrence_map) {
  map<long int, vector<long int> >::iterator itr_map;

  ofstream fout(path_to_co_occurrence_map.c_str());

  // loop through co-occurrence map and write co-occurrences into file
  for (itr_map = co_occurrences.begin(); itr_map != co_occurrences.end();
       itr_map++) {
    pair<long int, vector<long int> > entry = *itr_map;

    long int source_token = entry.first;

    fout << source_token << " ; ";

    vector<long int> target_tokens = entry.second;
    vector<long int>::iterator itr_token;

    for (itr_token = target_tokens.begin(); itr_token != target_tokens.end();
         itr_token++) {
      long int target_token = *itr_token;
      fout << target_token << " ";
    }
    fout << endl;
  }
  fout.close();
}

// writes the t-table into file
void Writer::writeTTable(const vector<WordProbs> &t_table,
                         const string &path_to_t_table) {
  vector<WordProbs>::const_iterator itr_map;

  ofstream fout(path_to_t_table.c_str());

  int source_token = 0;

  // loop though the t-table
  for (itr_map = t_table.begin(); itr_map != t_table.end(); itr_map++) {
    // for each source entry get the submap containing the corresponding target
    // token
    WordProbs target_submap = *itr_map;
    WordProbs::iterator itr_target_submap;

    // iterate through each submap and write source token corresponding target
    // token and probility into a file
    for (itr_target_submap = target_submap.begin();
         itr_target_submap != target_submap.end(); itr_target_submap++) {
      pair<long int, float> target_submap_entry = *itr_target_submap;

      long int target_token = target_submap_entry.first;
      float proba = target_submap_entry.second;
      fout << source_token << " " << target_token << " " << proba << endl;
    }
    source_token++;
  }
  fout.close();
}

// writes unigram frequencies into files
void Writer::writeUnigramFrequencies(WordProbs unigram_freq_of_source,
                                     WordProbs unigram_freq_of_target,
                                     string &path_to_unigram_frequencies) {
  string path = path_to_unigram_frequencies;
  string source_name = "Unigram_frequencies_of_source.txt";
  string source_path = path.append(source_name);

  path = path_to_unigram_frequencies;
  string target_name = "Unigram_frequencies_of_target.txt";
  string target_path = path.append(target_name);

  // create file to write in
  ofstream fout1(source_path.c_str());
  ofstream fout2(target_path.c_str());

  WordProbs::iterator itr_map;

  for (itr_map = unigram_freq_of_source.begin();
       itr_map != unigram_freq_of_source.end(); itr_map++) {
    pair<long int, float> f = *itr_map;
    fout1 << f.first << " " << f.second << endl;
  }

  for (itr_map = unigram_freq_of_target.begin();
       itr_map != unigram_freq_of_target.end(); itr_map++) {
    pair<long int, float> f = *itr_map;
    fout2 << f.first << " " << f.second << endl;
  }

  fout1.close();
  fout2.close();
}

// if only one-to-one correspondences are needed in the file containing the
// aligned sentences
/*
void Writer::writeAlignedData_1_to_1(string path_to_source_untokenized_data,
string path_to_target_untokenized_data, string path_to_alignment, string
path_to_aligned_source_data, string path_to_aligned_target_data, string
path_to_info_file)
{
        //get files in directory
        stack<string> filenames;

        filenames = Utilities::findAllFiles(path_to_alignment);

        //make output stream
        //file containing source sentences
        ofstream fout1(path_to_aligned_source_data.c_str());
        cout << "aligned source data in : " << path_to_aligned_source_data <<
endl;
        //file containing target sentences
        ofstream fout2(path_to_aligned_target_data.c_str());
        cout << "aligned target data in : " << path_to_aligned_target_data <<
endl;
        //info file
        ofstream fout3(path_to_info_file.c_str());

        while(!filenames.empty())
        {
                string filename = filenames.top();

                cout << "Processing file " << filename << endl;

                filenames.pop();

                Data my_data =
Reader::readEuroparlDocument(Utilities::findEuroparlFile(path_to_source_untokenized_data,filename).c_str(),Utilities::findEuroparlFile(path_to_target_untokenized_data,filename).c_str());

                //make hash map with data
                boost::unordered_map<long int, vector<string> >
source_sentences_map;
                boost::unordered_map<long int, vector<string> >
target_sentences_map;

                vector<Sentence> source_sentences =
my_data.getSourceSentences();
                vector<Sentence> target_sentences =
my_data.getTargetSentences();
                vector<Sentence> :: iterator itr_sentences;

                for(itr_sentences = source_sentences.begin(); itr_sentences !=
source_sentences.end(); itr_sentences++)
                {
                        Sentence sent = *itr_sentences;
                        //ignore insertions and deletions
                        if(sent.getPosition() == -1 || sent.getPosition() == -1)
                        {}
                        else
                        {pair<long int, vector<string> >
source_entry(sent.getPosition(), sent.getToken());
                        source_sentences_map.insert(source_entry);}
                }

                for(itr_sentences = target_sentences.begin(); itr_sentences !=
target_sentences.end(); itr_sentences++)
                {
                        Sentence sent = *itr_sentences;
                        //ignore insertions and deletions
                        if(sent.getPosition() == -1 || sent.getPosition() == -1)
                        {}
                        else
                        {
                        pair<long int, vector<string> >
target_entry(sent.getPosition(), sent.getToken());
                        target_sentences_map.insert(target_entry);}
                }

                //for debugging
                cout << "Size of source sentences " <<
source_sentences_map.size() << endl;
                cout << "Size of target sentences " <<
target_sentences_map.size() << endl;

                //path to aligned files
                string path = path_to_alignment;

                //open file
                string combined = path.append((filename).append(".txt"));
                ifstream fin1(combined.c_str());
                cout << "path to alignments " << combined << endl;

                //separator between source and target token
                string separator = ";";

                //store input line
                string line;

                //read alignments and find corresponding sentences
                while( getline ( fin1,line ) )
                {
                        char * sentCstr;
                        char * t;
                        sentCstr = new char [line.size()+1];
                        strcpy (sentCstr, line.c_str());
                        t=strtok(sentCstr," ");

                        vector<long int> source_sentence_ids;
                        vector<long int> target_sentence_ids;

                        while( t!= separator)
                        {
                                source_sentence_ids.push_back(atol(t));
                                t=strtok_r(NULL," ", &sp);
                        }

                        if( t == separator) // ignore separator
                        {
                                t=strtok_r(NULL," ", &sp);
                        }

                        while( t != NULL)
                        {
                                target_sentence_ids.push_back(atol(t));
                                t=strtok_r(NULL," ", &sp);
                        }

                        //iterate through source and target sentence ids
                        vector<long int> :: iterator itr_sent_ids;

                        //indicates if a sentence has been inserted or deleted
                        bool insertion_deletion = 0;
                        bool many_alignment = 0;

                        //ignore insertions or deletions
                        if(source_sentence_ids.front() == -1 ||
target_sentence_ids.front() == -1 )
                        {insertion_deletion = 1;}
                        else if(source_sentence_ids.size() != 1 ||
target_sentence_ids.size() != 1)
                        {many_alignment = 1;}
                        else
                        {
                                //write filename into info file
                                fout3 << filename << "\t";

                                //look for untokenized source sentences and
write into file
                                for(itr_sent_ids = source_sentence_ids.begin();
itr_sent_ids != source_sentence_ids.end(); itr_sent_ids++)
                                {
                                        long int sentence_id = *itr_sent_ids;
                                        //write each source sentence id into
info file
                                        if(itr_sent_ids ==
source_sentence_ids.end() -1)
                                        {fout3 << sentence_id;}
                                        else
                                        {fout3 << sentence_id << " ";}

                                        vector<string> token =
source_sentences_map.find(sentence_id)->second;
                                        vector<string> :: iterator itr_tok;

                                        for(itr_tok = token.begin(); itr_tok !=
token.end(); itr_tok++)
                                        {
                                                string tok = *itr_tok;
                                                fout1 << tok << " ";
                                        }
                                }
                                insertion_deletion = 0;
                                many_alignment = 0;
                                fout3 << "\t";
                        }
                        if(insertion_deletion == 0 && many_alignment == 0)
                        {fout1 << endl;}
                        //ignore deletions
                        if(source_sentence_ids.front() == -1 ||
target_sentence_ids.front() == -1)
                        {insertion_deletion = 1;}
                        else if(source_sentence_ids.size() != 1 ||
target_sentence_ids.size() != 1)
                        {many_alignment = 1;}
                        else
                        {
                                //write target sentence ids into info file
                                //look for untokenized target sentences and
write into file
                                for(itr_sent_ids = target_sentence_ids.begin();
itr_sent_ids != target_sentence_ids.end(); itr_sent_ids++)
                                {
                                        long int sentence_id = *itr_sent_ids;
                                        //write each source sentence id into
info file
                                        if(itr_sent_ids ==
target_sentence_ids.end() - 1)
                                        {fout3 << sentence_id;}
                                        else
                                        {fout3 << sentence_id << " ";}

                                        vector<string> token =
target_sentences_map.find(sentence_id)->second;
                                        vector<string> :: iterator itr_tok;
                                        for(itr_tok = token.begin(); itr_tok !=
token.end(); itr_tok++)
                                        {
                                                string tok = *itr_tok;
                                                fout2 << tok << " ";
                                        }
                                }
                                insertion_deletion = 0;
                                many_alignment = 0;
                        }
                        if(insertion_deletion == 0 && many_alignment == 0)
                        {fout2 << endl; fout3 << endl;}
                        source_sentence_ids.clear();
                        target_sentence_ids.clear();
                        delete [] sentCstr;
                        delete [] t;
                        line.clear();
                }
        }
}*/

/*
//for debugging : writes the t-table into file
void Writer::writePreparedTTable(boost::unordered_map<long int,
WordProbs > &prepared_t_table, string
&path_to_prepared_t_table, string &paragraph_name)
{
        //make output path
        string complete_path = path_to_prepared_t_table.append(paragraph_name);

        //make log file for each direction
        ofstream fout1(complete_path.c_str());

        LexProbs
:: iterator itr_ptt;

        for(itr_ptt = prepared_t_table.begin(); itr_ptt !=
prepared_t_table.end(); itr_ptt++)
        {
                pair<long int , WordProbs >
source_entry = *itr_ptt;

                long int source_token = source_entry.first;

                fout1 << "Source string : " << source_token << endl;

                WordProbs target_entry =
source_entry.second;
                WordProbs :: iterator itr_te;

                fout1 << "Targets :" << endl;

                for(itr_te = target_entry.begin(); itr_te != target_entry.end();
itr_te++)
                {
                        pair<long int, float> p = *itr_te;

                        long int target_token = p.first;
                        float t_proba = p.second;

                        fout1 << target_token << " : " << t_proba << endl;
                }
                fout1 << endl;
        }//end of loop through translation table
fout1.close();
}
*/
