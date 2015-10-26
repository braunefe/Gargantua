#include "LexicalDistributions.h"

LexicalDistributions::LexicalDistributions(const WordProbs &l_u_f_s,
                                           const WordProbs &l_u_f_t,
                                           const LexProbs &t_t) {
  setUnigramFrequenciesOfSource(l_u_f_s);
  setUnigramFrequenciesOfTarget(l_u_f_t);
  setTranslationTable(t_t);
}

WordProbs LexicalDistributions::getUnigramFrequenciesOfSource() {
  return unigram_frequencies_of_source;
}

void LexicalDistributions::setUnigramFrequenciesOfSource(const WordProbs &lus) {
  unigram_frequencies_of_source = lus;
}

WordProbs LexicalDistributions::getUnigramFrequenciesOfTarget() {
  return unigram_frequencies_of_target;
}

void LexicalDistributions::setUnigramFrequenciesOfTarget(const WordProbs &lut) {
  unigram_frequencies_of_target = lut;
}

LexProbs LexicalDistributions::getTranslationTable() const {
  return translation_table;
}

void LexicalDistributions::setTranslationTable(const LexProbs &t_t) {
  translation_table = t_t;
}

// training of model-1 : fractional counts for word translation pairs in a
// sentence that are not bigger than obtained by random choice are not added to
// the count for that translation pair (see Moore 2002)
float LexicalDistributions::trainModel1(string &path_to_co_occurrence_map,
                                        string &path_to_training_data,
                                        string &path_to_t_table,
                                        int nbr_iterations) {
  vector<WordProbs> t_table;

  // string to store input lines
  string line;

  // store number of target words
  int number_of_target_words = 0;
  int number_of_source_words = 0;

  // open file
  ifstream fin1(path_to_co_occurrence_map.c_str());
  cout << "reading " << path_to_co_occurrence_map << endl;
  if (!fin1) {
    cout << "Cannot open Co-Occurrence map" << endl;
    abort();
  }

  // null word is first line of co-occurrence map; null word co-occurrs with all
  // target token
  getline(fin1, line);

  // get token and build frequency map
  char *sentCstr;
  char *t;
  char *sp;
  sentCstr = new char[line.size() + 1];
  strcpy(sentCstr, line.c_str());
  t = strtok_r(sentCstr, " ", &sp);

  // separator between source and target token in token numbering
  string sep_1 = ";";

  // loop through source token
  while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
    t = strtok_r(NULL, " ", &sp);
  }

  // get rid of separator
  if (strcmp(t, sep_1.c_str()) == 0) {
    t = strtok_r(NULL, " ", &sp);
  }

  // loop through associated target token and count the number of occurrences
  while (t != NULL) {
    // get target token
    number_of_target_words++;

    // skip number of occurrences
    t = strtok_r(NULL, " ", &sp);
  }

  // clear temps
  line.clear();
  delete[] sentCstr;
  delete[] t;

  fin1.close();

  // compute uniform proba
  float uniform = 1.0 / (float)number_of_target_words;
  cout << "Number of target words " << number_of_target_words << endl;
  cout << "Uniform initialization at : " << uniform << endl;

  long int source_token = 0;
  long int target_token = 0;

  // open file
  ifstream fin2(path_to_co_occurrence_map.c_str());
  if (!fin2) {
    cout << "Cannot open co-occurrence map" << endl;
    abort();
  }

  // initialize t-table
  while (getline(fin2, line)) {
    // tokenize
    char *sentCstr;
    char *t;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // separator between source and target token in token numbering
    string sep_1 = ";";

    // loop through source token
    while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
      source_token = atol(t);
      number_of_source_words++;
      t = strtok_r(NULL, " ", &sp);
    }

    // get rid of separator
    if (strcmp(t, sep_1.c_str()) == 0) {
      t = strtok_r(NULL, " ", &sp);
    }

    WordProbs target_submap;

    // loop through associated target token and the number of occurrences
    while (t != NULL) {
      // get target token
      target_token = atol(t);

      // make submap for each target token
      pair<long int, float> t_submap_entry(target_token, uniform);
      target_submap.insert(t_submap_entry);

      t = strtok_r(NULL, " ", &sp);
    }
    // put target map into t_table
    t_table.push_back(target_submap);

    // clear temps
    line.clear();
    delete[] sentCstr;
    delete[] t;
  }
  fin2.close();

  cout << "Uniform Initialization done !" << endl;
  cout << "Size of t-table : " << t_table.size() << endl;

  // Model-1 training in X iterations : first iteration
  cout << "EM first iteration " << endl;

  vector<long int> source_chunk;
  vector<long int> target_chunk;

  float t_prob = 0.0;

  // loop through training data and make count table

  // for debugging
  // cout << "Making count table ... " << endl;

  // initialize proba for target
  vector<float> total_proba_target(number_of_target_words + 1, 0.0);

  // initialize count table
  WordProbs submap;
  vector<WordProbs> c_table(number_of_source_words + 1, submap);

  // initialize proba for source
  vector<float> total_proba_source(number_of_source_words + 1, 0.0);

  // get filenames
  stack<string> filenames;
  filenames = Utilities::findAllFiles(path_to_training_data);

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // for debugging
    // cout << "processing file : " << filename << endl;

    string path = path_to_training_data;

    // open file
    ifstream fin3(path.append(filename.append(".txt")).c_str());

    if (!fin3) {
      cout << "Couldn´t open input file" << path << endl;
    } else {
      // go through file and put source and target sentences into maps
      while (getline(fin3, line)) {
        // for debugging
        // cout << "Processing line " << line_counter++ << endl;

        // get token
        char *sentCstr_1;
        char *t_1;
        char *sp;
        sentCstr_1 = new char[line.size() + 1];
        strcpy(sentCstr_1, line.c_str());
        t_1 = strtok_r(sentCstr_1, " ", &sp);

        while (t_1 != NULL)  // loop through source token
        {
          source_chunk.push_back(atol(t_1));
          t_1 = strtok_r(NULL, " ", &sp);
        }

        line.clear();

        getline(fin3, line);

        // get token
        char *sentCstr_2;
        char *t_2;
        sentCstr_2 = new char[line.size() + 1];
        strcpy(sentCstr_2, line.c_str());
        t_2 = strtok_r(sentCstr_2, " ", &sp);

        while (t_2 != NULL)  // loop through target token
        {
          target_chunk.push_back(atol(t_2));
          t_2 = strtok_r(NULL, " ", &sp);
        }

        // iterate through sentence pairs and make source counts
        vector<long int>::iterator itr_source_sent;
        vector<long int>::iterator itr_target_sent;

        // for debugging
        // cout << "Making target count table " << endl;
        for (itr_target_sent = target_chunk.begin();
             itr_target_sent != target_chunk.end(); itr_target_sent++) {
          long int target_token = *itr_target_sent;
          total_proba_target.at(target_token) = 0.0;

          for (itr_source_sent = source_chunk.begin();
               itr_source_sent != source_chunk.end(); itr_source_sent++) {
            long int source_token = *itr_source_sent;
            t_prob += t_table.at(source_token).find(target_token)->second;
          }
          // put proba into slot for target token (only if not null (after
          // pruning))
          if (t_prob != 0.0) {
            total_proba_target.at(target_token) = t_prob;
          }
          // reinitialize t_proba
          t_prob = 0.0;
        }  // end of for through source and target sentence words

        // cout << "Making count table " << endl;
        for (itr_target_sent = target_chunk.begin();
             itr_target_sent != target_chunk.end(); itr_target_sent++) {
          long int target_token = *itr_target_sent;

          for (itr_source_sent = source_chunk.begin();
               itr_source_sent != source_chunk.end(); itr_source_sent++) {
            long int source_token = *itr_source_sent;
            t_prob = t_table.at(source_token).find(target_token)->second /
                     total_proba_target.at(target_token);

            // put proba into count table
            // check if token is already in count table
            if (c_table.at(source_token).find(target_token) !=
                c_table.at(source_token).end()) {  // if yes, add proba

              c_table.at(source_token)[target_token] += t_prob;
            } else {  // add target token to submap
              // make pair containing new proba
              pair<long int, float> new_prob(target_token, t_prob);
              c_table.at(source_token).insert(new_prob);
            }
            total_proba_source.at(source_token) += t_prob;
          }  // end of loop through source sentences
        }  // end of loop through target sentences
        // clear temps
        line.clear();
        source_chunk.clear();
        target_chunk.clear();
        delete[] sentCstr_1;
        delete[] t_1;
        delete[] sentCstr_2;
        delete[] t_2;
      }  // end of while
      fin3.close();
    }  // end of else
  }  // end of loop through filenames

  // clear old t-table
  total_proba_target.clear();  // set total source probas to 0
  t_table.clear();

  // loop through co-occurrence map and make t-table
  cout << "Making t-table ... " << endl;

  // initialize t-table
  WordProbs t_submap;
  for (int i = 0; i < number_of_source_words + 1; i++) {
    t_table.push_back(t_submap);
  }

  // open file
  ifstream fin6(path_to_co_occurrence_map.c_str());
  if (!fin6) {
    cout << "Cannot open Co-Occurrence map." << path_to_co_occurrence_map
         << endl;
    abort();
  }

  // loop through co-occurrence map
  while (getline(fin6, line)) {
    // tokenize
    char *sentCstr;
    char *t;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // separator between source and target token in token numbering
    string sep_1 = ";";

    // loop through source token
    while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
      source_token = atol(t);
      t = strtok_r(NULL, " ", &sp);
    }

    // get rid of separator
    if (strcmp(t, sep_1.c_str()) == 0) {
      t = strtok_r(NULL, " ", &sp);
    }

    // make submap for t_ table
    WordProbs target_map;

    // loop through associated target token and count the number of occurrences
    while (t != NULL) {
      // get target token
      target_token = atol(t);

      // initialize t_proba
      float t_prob = 0.0;

      // if token is in count table, make t-proba by dividing by proba for all
      // target token
      if (c_table.at(source_token).find(target_token) !=
          c_table.at(source_token).end()) {
        t_prob = c_table.at(source_token).find(target_token)->second /
                 total_proba_source.at(source_token);

        pair<long int, float> target_entry(target_token, t_prob);
        t_table.at(source_token).insert(target_entry);
      }
      t = strtok_r(NULL, " ", &sp);
    }
    // clear temps
    line.clear();
    delete[] sentCstr;
    delete[] t;
  }
  fin6.close();
  // for debugging
  // cout << "Size of t-table " << t_table.size() << endl;

  // Model-1 training in X iterations : all iterations after the first
  for (int c = 1; c < nbr_iterations; c++) {
    cout << "EM iteration after the first: " << c << endl;

    vector<long int> source_chunk;
    vector<long int> target_chunk;

    float t_prob;

    c_table.clear();  // set the count table to 0
    total_proba_source.clear();  // set total target probas to 0
    total_proba_target.clear();

    // initialize proba for target
    for (int i = 0; i < number_of_target_words + 1; i++) {
      total_proba_target.push_back(0.0);
    }

    // initialize count table
    WordProbs c_submap;
    for (int i = 0; i < number_of_source_words + 1; i++) {
      c_table.push_back(c_submap);
    }

    // initialize proba for source
    for (int i = 0; i < number_of_source_words + 1; i++) {
      total_proba_source.push_back(0.0);
    }

    // loop through source and target chunks
    stack<string> filenames;

    // get filenames
    filenames = Utilities::findAllFiles(path_to_training_data);

    while (!filenames.empty()) {
      string filename = filenames.top();
      filenames.pop();

      // for debugging
      // cout << "processing file : " << filename << endl;

      string path = path_to_training_data;

      // open file
      ifstream fin1(path.append(filename.append(".txt")).c_str());

      if (!fin1) {
        cout << "Couldn´t open input file" << path << endl;
        abort();
      } else {
        // go through file and put source and target sentences into maps
        while (getline(fin1, line)) {
          // get token
          char *sentCstr_1;
          char *t_1;
          char *sp;
          sentCstr_1 = new char[line.size() + 1];
          strcpy(sentCstr_1, line.c_str());
          t_1 = strtok_r(sentCstr_1, " ", &sp);

          while (t_1 != NULL)  // loop through source token
          {
            source_chunk.push_back(atol(t_1));
            t_1 = strtok_r(NULL, " ", &sp);
          }

          line.clear();

          getline(fin1, line);

          // get token
          char *sentCstr_2;
          char *t_2;
          sentCstr_2 = new char[line.size() + 1];
          strcpy(sentCstr_2, line.c_str());
          t_2 = strtok_r(sentCstr_2, " ", &sp);

          while (t_2 != NULL)  // loop through target token
          {
            target_chunk.push_back(atol(t_2));
            t_2 = strtok_r(NULL, " ", &sp);
          }
          // iterate through sentence pairs and make source counts
          vector<long int>::iterator itr_source_sent;
          vector<long int>::iterator itr_target_sent;

          // loop through training data and total proba for source
          // cout << "Making target count table ..." << endl;
          for (itr_target_sent = target_chunk.begin();
               itr_target_sent != target_chunk.end(); itr_target_sent++) {
            long int target_token = *itr_target_sent;

            total_proba_target.at(target_token) = 0.0;

            for (itr_source_sent = source_chunk.begin();
                 itr_source_sent != source_chunk.end(); itr_source_sent++) {
              long int source_token = *itr_source_sent;

              if (t_table.at(source_token).find(target_token) !=
                  t_table.at(source_token).end()) {
                t_prob += t_table.at(source_token).find(target_token)->second;
                // for debugging
                // cout << "T-prob " << t_prob << endl;
              }
            }
            // put proba into slot for target token (only if not null (after
            // pruning))
            if (t_prob != 0.0) {
              total_proba_target.at(target_token) = t_prob;
            }
            // reinitialize t_proba
            t_prob = 0.0;
          }

          // loop through training data and make count table
          // cout << "Making count table ... " << endl;
          for (itr_target_sent = target_chunk.begin();
               itr_target_sent != target_chunk.end(); itr_target_sent++) {
            long int target_token = *itr_target_sent;

            for (itr_source_sent = source_chunk.begin();
                 itr_source_sent != source_chunk.end(); itr_source_sent++) {
              long int source_token = *itr_source_sent;

              // check if target token is in submap
              if (t_table.at(source_token).find(target_token) !=
                  t_table.at(source_token).end()) {
                t_prob = t_table.at(source_token).find(target_token)->second /
                         total_proba_target.at(target_token);
              }

              // check if proba is big enough (if smaller than random do not
              // add)
              if (t_prob > 1.0 / (double)source_chunk.size()) {
                // put proba into count table
                // check if token is already in count table
                if (c_table.at(source_token).find(target_token) !=
                    c_table.at(source_token).end()) {  // if yes, add proba
                  c_table.at(source_token)[target_token] += t_prob;
                } else {  // add target token to submap
                  // make pair containing new proba
                  pair<long int, float> new_prob(target_token, t_prob);
                  c_table.at(source_token).insert(new_prob);
                }

                // put proba into total source probas
                total_proba_source.at(source_token) += t_prob;
              } else  // otherwise assign count to null word
              {
                if (c_table.at(0).find(target_token) !=
                    c_table.at(0).end()) {  // if yes, add proba
                  c_table.at(0)[target_token] += t_prob;
                } else {  // add target token to submap
                  // make pair containing new proba
                  pair<long int, float> new_prob(target_token, t_prob);
                  c_table.at(source_token).insert(new_prob);
                }

                // put proba into total source probas
                // check if token is already in target map
                total_proba_source.at(0) += t_prob;
              }
              t_prob = 0.0;
            }  // end of loop through source
          }  // end of loop through sentence pairs
          // clear temps
          line.clear();
          source_chunk.clear();
          target_chunk.clear();
          delete[] sentCstr_1;
          delete[] t_1;
          delete[] sentCstr_2;
          delete[] t_2;
        }  // end of while through files
        fin1.close();
      }  // end of else
    }  // end of while
    // clear old t-table
    total_proba_target.clear();  // set total source probas to 0
    t_table.clear();

    // loop through co-occurrence map and make t-table
    cout << "Making t-table... " << endl;

    // initialize t-table
    WordProbs t_submap;
    for (int i = 0; i < number_of_source_words + 1; i++) {
      t_table.push_back(t_submap);
    }

    // open file
    ifstream fin6(path_to_co_occurrence_map.c_str());
    if (!fin6) {
      cout << "Cannot open Co-Occurrence map." << path_to_co_occurrence_map
           << endl;
      abort();
    }

    // loop through co-occurrence map
    while (getline(fin6, line)) {
      // get token and build frequency map
      char *sentCstr;
      char *t;
      sentCstr = new char[line.size() + 1];
      strcpy(sentCstr, line.c_str());
      t = strtok_r(sentCstr, " ", &sp);

      // separator between source and target token in token numbering
      string sep_1 = ";";

      // loop through source token
      while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
        source_token = atol(t);
        t = strtok_r(NULL, " ", &sp);
      }

      // get rid of separator
      if (strcmp(t, sep_1.c_str()) == 0) {
        t = strtok_r(NULL, " ", &sp);
      }

      // make submap for t_ table
      WordProbs target_map;

      // loop through associated target token and count the number of
      // occurrences
      while (t != NULL) {
        // get target token
        target_token = atol(t);
        t = strtok_r(NULL, " ", &sp);

        // initialize t_proba
        float t_prob = 0.0;

        // if target token is in count table compute t_prob and put into t-table
        if (c_table.at(source_token).find(target_token) !=
            c_table.at(source_token).end()) {
          t_prob = c_table.at(source_token).find(target_token)->second /
                   total_proba_source.at(source_token);

          pair<long int, float> target_entry(target_token, t_prob);
          t_table.at(source_token).insert(target_entry);
        }
      }
      // clear temps
      line.clear();
      delete[] sentCstr;
      delete[] t;
    }  // end of loop through occurrence map
    fin6.close();
    cout << "Size of t-table : " << t_table.size() << endl;
  }  // end of EM iterations

  // write t-table
  Writer::writeTTable(t_table, path_to_t_table);
  return uniform;
}

// renumbers t-table according to numbering maps from training data and writes
// new table into file
void LexicalDistributions::renumberTTable(
    string &path_to_source_token_numbering,
    string &path_to_target_token_numbering, string &path_to_t_table,
    string &path_to_renumbered_table) {
  // read numbering maps from disk
  map<long int, long int> source_token_numbering;
  map<long int, long int> target_token_numbering;

  // store input line
  string line;

  // token in each line of the numbering map
  long int original_token = 0;
  long int numbered_token = 0;

  // stream to re-numbered t-table
  ofstream fout(path_to_renumbered_table.c_str());

  // load source token numbering map
  ifstream fin1(path_to_source_token_numbering.c_str());
  if (!fin1) {
    cout << "Cannot open source token numbering map."
         << path_to_source_token_numbering << endl;
    abort();
  }

  while (getline(fin1, line)) {
    // tokenize
    char *sentCstr;
    char *t;
    char *sp;

    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // separator between source and target token in token numbering
    string sep_1 = ":";

    // loop through source token
    while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
      original_token = atol(t);
      t = strtok_r(NULL, " ", &sp);
    }

    // get rid of separator
    if (strcmp(t, sep_1.c_str()) == 0) {
      t = strtok_r(NULL, " ",
                   &sp);  // insert null and other words into source token
    }

    // loop through associated target token and count the number of occurrences
    while (t != NULL) {
      // get target token
      numbered_token = atol(t);
      t = strtok_r(NULL, " ", &sp);
    }
    pair<long int, long int> source_numbering_entry(numbered_token,
                                                    original_token);
    source_token_numbering.insert(source_numbering_entry);
    // clear temps
    line.clear();
    delete[] sentCstr;
    delete[] t;
  }  // end of loop through occurrence map
  fin1.close();

  // load source token numbering map
  ifstream fin2(path_to_target_token_numbering.c_str());
  if (!fin2) {
    cout << "Cannot open source token numbering map."
         << path_to_target_token_numbering << endl;
    abort();
  }

  while (getline(fin2, line)) {
    // get token and build source token numbering map
    char *sentCstr;
    char *t;
    char *sp;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // separator between source and target token in token numbering
    string sep_1 = ":";

    // loop through source token
    while (t != NULL && (strcmp(t, sep_1.c_str()) != 0)) {
      original_token = atol(t);
      t = strtok_r(NULL, " ", &sp);
    }

    // get rid of separator
    if (strcmp(t, sep_1.c_str()) == 0) {
      t = strtok_r(NULL, " ", &sp);
    }

    // loop through associated target token and count the number of occurrences
    while (t != NULL) {
      // get target token
      numbered_token = atol(t);
      t = strtok_r(NULL, " ", &sp);
    }
    pair<long int, long int> target_numbering_entry(numbered_token,
                                                    original_token);
    target_token_numbering.insert(target_numbering_entry);
    // clear temps
    line.clear();
    delete[] sentCstr;
    delete[] t;
  }  // end of loop through occurrence map
  fin2.close();

  // regex to extract source word target word and proba from t-table
  regex space("\\s+");

  // make sure line is cleared
  line.clear();

  // open t-table
  ifstream fin3(path_to_t_table.c_str());
  if (!fin3) {
    cout << "Cannot open T-Table" << endl;
    abort();
  }

  // go through pruned t-table line by line
  while (getline(fin3, line)) {
    // extract token from line
    sregex_token_iterator i(line.begin(), line.end(), space, -1);
    sregex_token_iterator iend;

    string source_token_id_str = *i++;
    string target_token_id_str = *i++;
    string proba_str = *i;

    long int source_token_id = atol(source_token_id_str.c_str());
    long int target_token_id = atol(target_token_id_str.c_str());
    float proba = strtod(proba_str.c_str(), NULL);

    // replace source token id by original id
    if (source_token_numbering.find(source_token_id) !=
        source_token_numbering.end()) {
      long int source_token =
          source_token_numbering.find(source_token_id)->second;
      fout << source_token << " ";
    } else {
      cout << "Source token not found" << endl;
    }

    // replace target token id by original id
    if (target_token_numbering.find(target_token_id) !=
        target_token_numbering.end()) {
      long int target_token =
          target_token_numbering.find(target_token_id)->second;
      fout << target_token << " ";
    } else {
      cout << "Target token not found" << endl;
    }
    // display corresponding proba
    fout << proba << endl;
    line.clear();
  }
  fin3.close();
}

// computes unigram frequencies of words for the whole corpus
void LexicalDistributions::computeUnigramFrequencies(
    string &input_path_source, string &input_path_target,
    string &path_to_unigram_frequencies) {
  // store all occurrence of source and target token
  vector<long int> source_token;
  vector<long int> target_token;

  // iterator through token
  vector<long int>::iterator itr_token;

  // map containing new unigram frequencies
  WordProbs unigram_freq_of_source;
  WordProbs unigram_freq_of_target;
  WordProbs::iterator itr_map;

  // get names of files in source and target input directories
  stack<string> filenames;

  filenames = Utilities::findAllFiles(input_path_source);  // source and target
                                                           // have the same name
                                                           // : search only in 1
                                                           // directory to find
                                                           // filenames

  // loop through files in the corpus
  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // For debugging
    // cout << "Processing file " << filename << endl;

    // read Europarl input document (source and target documents)
    Data d = Reader::readNumberedEuroparlDocument(
        Utilities::findEuroparlFile(input_path_source, filename).c_str(),
        Utilities::findEuroparlFile(input_path_target, filename).c_str());

    // get source and target data
    vector<Sentence> source_sentences = d.getSourceSentences();
    vector<Sentence> target_sentences = d.getTargetSentences();

    vector<Sentence>::iterator itr_sentence;

    // loop through source sentences and put all token into vector
    for (itr_sentence = source_sentences.begin();
         itr_sentence != source_sentences.end(); itr_sentence++) {
      Sentence sent = *itr_sentence;
      vector<long int> tokens = sent.getTokenInts();
      for (itr_token = tokens.begin(); itr_token != tokens.end(); itr_token++) {
        long int token = *itr_token;
        source_token.push_back(token);
      }
    }  // end of loop for source sentences

    // loop through target sentences and put all token into vector
    for (itr_sentence = target_sentences.begin();
         itr_sentence != target_sentences.end(); itr_sentence++) {
      Sentence sent = *itr_sentence;
      vector<long int> tokens = sent.getTokenInts();

      for (itr_token = tokens.begin(); itr_token != tokens.end(); itr_token++) {
        long int token = *itr_token;
        target_token.push_back(token);
      }
    }  // end of loop for target sentences
  }  // end of loop through files

  // sort source and target token
  sort(source_token.begin(), source_token.end());
  sort(target_token.begin(), target_token.end());

  itr_token = source_token.begin();
  while (itr_token != source_token.end()) {
    long int source_tok = *itr_token;

    float occurrence_counter = 0.0;
    while ((binary_search(itr_token, source_token.end(),
                          source_tok)))  // check if duplicates
    {
      occurrence_counter++;
      itr_token++;
    }

    pair<long int, float> source_occurrence_entry(source_tok,
                                                  occurrence_counter);
    unigram_freq_of_source.insert(source_occurrence_entry);
  }

  itr_token = target_token.begin();
  while (itr_token != target_token.end()) {
    long int target_tok = *itr_token;

    float occurrence_counter = 0.0;
    while ((binary_search(itr_token, target_token.end(),
                          target_tok)))  // check if duplicates
    {
      occurrence_counter++;
      itr_token++;
    }

    pair<long int, float> target_occurrence_entry(target_tok,
                                                  occurrence_counter);
    unigram_freq_of_target.insert(target_occurrence_entry);
  }

  // For debugging
  // cout << "Source and target frequency maps done " << endl;
  // cout << "Size of source frequency map " << unigram_freq_of_source.size() <<
  // endl;
  // cout << "Size of target frequency map " << unigram_freq_of_target.size() <<
  // endl;

  // compute number of source and target token to compute unigram frequency of
  // "other word"
  float number_of_source_token = (float)source_token.size();

  // For debugging
  // cout << "Number of source token " << number_of_source_token << endl;
  float number_of_target_token = (float)target_token.size();

  // For debugging
  // cout << "Number of target token " << number_of_target_token << endl;

  // loop through source frequency map collect source words that occurr the same
  // number of times
  boost::unordered_map<float, set<long int> > words_for_source_occurrence_count;

  for (itr_map = unigram_freq_of_source.begin();
       itr_map != unigram_freq_of_source.end(); itr_map++) {
    pair<long int, float> entry = *itr_map;
    float ratio = entry.second / number_of_source_token;
    unigram_freq_of_source.find(entry.first)->second = ratio;
  }

  // loop through target frequency map collect target words that occurr the same
  // number of times
  boost::unordered_map<float, set<long int> > words_for_target_occurrence_count;

  for (itr_map = unigram_freq_of_target.begin();
       itr_map != unigram_freq_of_target.end(); itr_map++) {
    pair<long int, float> entry = *itr_map;
    float ratio = entry.second / number_of_target_token;
    unigram_freq_of_target.find(entry.first)->second = ratio;
  }

  // fix unigram frequency of "other token" to 1/number of source or target
  // words
  unigram_freq_of_source.find(1)->second = 10.0f / number_of_source_token;
  unigram_freq_of_target.find(1)->second = 10.0f / number_of_target_token;

  // For debugging
  // cout << "Size of unigram source " << unigram_freq_of_source.size() << endl;
  // cout << "Size of unigram target " << unigram_freq_of_target.size() << endl;
  Writer::writeUnigramFrequencies(unigram_freq_of_source,
                                  unigram_freq_of_target,
                                  path_to_unigram_frequencies);
}

// prepares a t-table for each document by filtering out words that are in the
// document (document = numbered_data_stack)
void LexicalDistributions::prepareTTable(const vector<Data>& numbered_data_stack,
                                         LexProbs &t_table) {
  LexProbs prepared_t_table;

  // set of different source and target token
  boost::unordered_set<long int> source_token;
  boost::unordered_set<long int> target_token;

  // insert null and other words into source token
  source_token.insert(0);
  source_token.insert(1);

  // insert other word into target token
  target_token.insert(1);

  // loop through data stack
  for (const auto& numbered_data : numbered_data_stack) {
    // get the source and target sentences
    vector<Sentence> source_sentences = numbered_data.getSourceSentences();
    vector<Sentence> target_sentences = numbered_data.getTargetSentences();

    vector<Sentence>::iterator itr_sentences;

    // iterate over source sentences and put source token into set
    for (itr_sentences = source_sentences.begin();
         itr_sentences != source_sentences.end(); itr_sentences++) {
      Sentence s = *itr_sentences;
      vector<long int> token = s.getTokenInts();

      vector<long int>::iterator itr_token;

      for (itr_token = token.begin(); itr_token != token.end(); itr_token++) {
        long int token = *itr_token;
        source_token.insert(token);
      }
    }

    // iterate over target sentences and put source token into set
    for (itr_sentences = target_sentences.begin();
         itr_sentences != target_sentences.end(); itr_sentences++) {
      Sentence s = *itr_sentences;
      vector<long int> token = s.getTokenInts();

      vector<long int>::iterator itr_token;

      for (itr_token = token.begin(); itr_token != token.end(); itr_token++) {
        long int token = *itr_token;
        target_token.insert(token);
      }
    }
  }

  // iterate through t-table
  LexProbs::iterator itr_map;
  for (itr_map = t_table.begin(); itr_map != t_table.end(); itr_map++) {
    pair<long int, WordProbs> entry = *itr_map;
    long int source_token_id = entry.first;

    WordProbs::iterator itr_submap;
    for (itr_submap = entry.second.begin(); itr_submap != entry.second.end();
         itr_submap++) {
      pair<long int, float> sub_entry = *itr_submap;
      long int target_token_id = sub_entry.first;
      float proba = sub_entry.second;

      if (source_token.find(source_token_id) != source_token.end() &&
          target_token.find(target_token_id) != target_token.end()) {
        // check if source token is in map
        if (prepared_t_table.find(source_token_id) != prepared_t_table.end()) {
          // check for target token not necessary since all pairs (s,t) are
          // unique
          pair<long int, float> submap_entry(target_token_id, proba);
          prepared_t_table.find(source_token_id)->second.insert(submap_entry);
        } else {  // make target submap
          WordProbs submap;
          pair<long int, float> submap_entry(target_token_id, proba);
          submap.insert(submap_entry);
          pair<long int, WordProbs> entry(source_token_id, submap);
          prepared_t_table.insert(entry);
        }
      }
    }
  }
  // keep prepared t_table into memory
  setTranslationTable(prepared_t_table);

  // for debugging : write t-table
  // string path_to_prepared_tables =
  // "./lexical_distributions/prepared_t_tables/";
  // string table_name = "current_table.txt";
  // Writer::writePreparedTTable(prepared_t_table, path_to_prepared_tables,
  // table_name);
}

// prepares unigram frequencies for each document by filtering out words that
// are in the document (document = numbered_data_stack)
void LexicalDistributions::prepareUnigramFrequencies(
    const vector<Data>& numbered_data_stack,
    string &path_to_unigram_frequencies_of_source,
    string &path_to_unigram_frequencies_of_target) {
  WordProbs unigram_freq_of_source;
  WordProbs unigram_freq_of_target;

  // set of different source and target token
  boost::unordered_set<long int> source_token;
  boost::unordered_set<long int> target_token;

  // regex to extract token identity from token numbering
  regex separator(":");

  // regex to extract source word target word and proba from t-table
  regex space("\\s+");

  // insert null word into source token
  source_token.insert(0);

  // loop through data stack
  for (const auto& numbered_data : numbered_data_stack) {
    // get source and target sentences
    vector<Sentence> source_sentences = numbered_data.getSourceSentences();
    vector<Sentence> target_sentences = numbered_data.getTargetSentences();

    vector<Sentence>::iterator itr_sentences;

    // iterate over source sentences and put source token into vector
    for (itr_sentences = source_sentences.begin();
         itr_sentences != source_sentences.end(); itr_sentences++) {
      Sentence s = *itr_sentences;
      vector<long int> token = s.getTokenInts();

      vector<long int>::iterator itr_token;

      for (itr_token = token.begin(); itr_token != token.end(); itr_token++) {
        long int token = *itr_token;
        source_token.insert(token);
      }
    }

    // iterate over target sentences and put token into vector
    for (itr_sentences = target_sentences.begin();
         itr_sentences != target_sentences.end(); itr_sentences++) {
      Sentence s = *itr_sentences;
      vector<long int> token = s.getTokenInts();

      vector<long int>::iterator itr_token;

      for (itr_token = token.begin(); itr_token != token.end(); itr_token++) {
        long int token = *itr_token;
        target_token.insert(token);
      }
    }
  }

  // store input line
  string line;

  // open unigram frequencies
  ifstream fin1(path_to_unigram_frequencies_of_source.c_str());
  if (!fin1) {
    cout << "Cannot open Unigram frequencies of source at : "
         << path_to_unigram_frequencies_of_source << endl;
    abort();
  }

  // go through source pruned t-table
  while (getline(fin1, line)) {
    // get token and build frequency map
    char *sentCstr;
    char *t;
    char *sp;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // each line consists in 3 token
    int counter = 0;
    long int source_token_id = 0;
    float proba = 0.0f;

    while (t != NULL) {
      counter++;

      if (counter == 1) {
        source_token_id = atol(t);
        t = strtok_r(NULL, " ", &sp);
      }
      if (counter == 2) {
        proba = Utilities::stof(t);
        t = strtok_r(NULL, " ", &sp);
      }
    }
    delete[] sentCstr;
    delete[] t;

    if (source_token.find(source_token_id) != source_token.end()) {
      pair<long int, float> freq_entry(source_token_id, proba);
      unigram_freq_of_source.insert(freq_entry);
    }
  }
  fin1.close();

  // open unigram frequencies
  ifstream fin2(path_to_unigram_frequencies_of_target.c_str());
  if (!fin2) {
    cout << "Cannot open Unigram frequencies of target at : "
         << path_to_unigram_frequencies_of_target << endl;
    abort();
  }

  // go through source pruned t-table
  while (getline(fin2, line)) {
    // get token and build frequency map
    char *sentCstr;
    char *t;
    char *sp;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // each line consists in 3 token
    int counter = 0;
    long int target_token_id = 0;
    float proba = 0.0f;

    while (t != NULL) {
      counter++;

      if (counter == 1) {
        target_token_id = atol(t);
        t = strtok_r(NULL, " ", &sp);
      }
      if (counter == 2) {
        proba = Utilities::stof(t);  //_atold not found
        t = strtok_r(NULL, " ", &sp);
      }
    }
    delete[] sentCstr;
    delete[] t;

    if (target_token.find(target_token_id) != target_token.end()) {
      pair<long int, float> freq_entry(target_token_id, proba);
      unigram_freq_of_target.insert(freq_entry);
    }
  }
  fin2.close();

  // keep prepared frequencies into memory
  setUnigramFrequenciesOfSource(unigram_freq_of_source);
  setUnigramFrequenciesOfTarget(unigram_freq_of_target);

  // for debugging
  // string path_to_unigram_frequencies =
  // "./lexical_distributions/unigram_language_model/prepared_unigram_frequencies/";
  // Writer::writeUnigramFrequencies(unigram_freq_of_source,
  // unigram_freq_of_target, path_to_unigram_frequencies);
}

// loads the t-table into memory
LexProbs LexicalDistributions::loadTTable(string &path_to_t_table) {
  LexProbs t_table;

  // store input line
  string line;

  // open t-table
  ifstream fin3(path_to_t_table.c_str());
  if (!fin3) {
    cout << "Cannot open T-Table" << endl;
    abort();
  }

  // read token in input file
  // go through source pruned t-table
  while (getline(fin3, line)) {
    // tokenize
    char *sentCstr;
    char *t;
    char *sp;
    sentCstr = new char[line.size() + 1];
    strcpy(sentCstr, line.c_str());
    t = strtok_r(sentCstr, " ", &sp);

    // each line consists of 3 token
    int counter = 0;
    long int source_token_id = 0;
    long int target_token_id = 0;
    float proba = 0.0f;

    while (t != NULL) {
      counter++;

      if (counter == 1) {
        source_token_id = atol(t);
        t = strtok_r(NULL, " ", &sp);
      }
      if (counter == 2) {
        target_token_id = atol(t);
        t = strtok_r(NULL, " ", &sp);
      }
      if (counter == 3) {
        proba = Utilities::stof(t);  //_atold not found
        t = strtok_r(NULL, " ", &sp);
      }
    }
    delete[] sentCstr;
    delete[] t;

    // check if source token is in map
    if (t_table.find(source_token_id) != t_table.end()) {
      // check for target token not necessary since all pairs (s,t) are unique
      pair<long int, float> submap_entry(target_token_id, proba);
      t_table.find(source_token_id)->second.insert(submap_entry);
    } else {  // make target submap
      WordProbs submap;
      pair<long int, float> submap_entry(target_token_id, proba);
      submap.insert(submap_entry);
      pair<long int, WordProbs> entry(source_token_id, submap);
      t_table.insert(entry);
    }
  }
  fin3.close();
  return t_table;
}

float LexicalDistributions::getTTableEntry(const long int stok,
                                           const long int ttok) const {
  auto s_lex = translation_table.find(stok);
  if (s_lex == translation_table.end()) {
    s_lex = translation_table.find(0);
  }
  const auto t_lex = s_lex->second.find(ttok);
  if (t_lex == s_lex->second.end()) {
    return 0.0000001f;
  }
  return t_lex->second;
}

vector<float> LexicalDistributions::getTTableEntries(
    const long int stok, const vector<long int> &ttoks) const {
  vector<float> res;
  res.reserve(ttoks.size());

  auto s_lex = translation_table.find(stok);
  if (s_lex == translation_table.end()) {
    s_lex = translation_table.find(0);
  }

  for (const auto ttok : ttoks) {
    const auto t_lex = s_lex->second.find(ttok);
    if (t_lex != s_lex->second.end()) {
      res.push_back(t_lex->second);
    } else {
      res.push_back(0.0000001f);
    }
  }
  return res;
}
