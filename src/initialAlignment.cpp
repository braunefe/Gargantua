#include "InitialAlignment.h"

InitialAlignment::InitialAlignment(const list<pair<int, int> >& f_d) {
  setDiagonal(f_d);
}

list<pair<int, int> > InitialAlignment::getDiagonal() const { return diagonal; }

void InitialAlignment::setDiagonal(const list<pair<int, int> >& f_d) {
  diagonal = f_d;
}

stack<Chunk> InitialAlignment::getAlignment() { return alignment; }
void InitialAlignment::setAlignment(stack<Chunk> a) { alignment = a; }

bool InitialAlignment::getExpandBand1() const { return expand_band_1; }
void InitialAlignment::setExpandBand1(bool e_b_1) { expand_band_1 = e_b_1; }

bool InitialAlignment::getExpandBand2() const { return expand_band_2; }

void InitialAlignment::setExpandBand2(bool e_b_2) { expand_band_2 = e_b_2; }

bool InitialAlignment::getDynamicPrgrammingState() const {
  return dynamic_programming_done;
}

void InitialAlignment::setDynamicProgrammingState(bool d_p_d) {
  dynamic_programming_done = d_p_d;
}

// sentence length and lexical based distance function as defined in Moore 2002
// but restricted to 1-to-0/0-to-1 and 1-to-1 alignments and with Viterbi model
// 1 instead of summing up over all alignments
float InitialAlignment::distanceFunctionMoore(
    const LengthDistributions &length_distributions,
    const LexicalDistributions &lexical_distributions, vector<long int> &st,
    vector<long int> &tt) {
  int x1 = (int)st.size();
  int y1 = (int)tt.size();

  float prior01 = 0.03f;
  float prior10 = 0.03f;
  float prior11 = 0.94f;

  if (x1 == 0)  // insertion
  {
    // compute length statistics
    float relft =
        length_distributions.relative_frequencies_of_target.find(y1)->second;
    float lengthDist = -log(relft) - log(prior01);

    vector<long int>::iterator itr_target_token;

    // compute unigram frequencies
    float uf = 0.0f;
    float wordDist = 0.0f;

    // iterate through target token and sum up neg log unigram frequencies
    for (itr_target_token = tt.begin(); itr_target_token != tt.end();
         itr_target_token++) {
      long int token = *itr_target_token;

      if (lexical_distributions.unigram_frequencies_of_target.find(token) !=
          lexical_distributions.unigram_frequencies_of_target.end()) {
        uf = lexical_distributions.unigram_frequencies_of_target.find(token)
                 ->second;
      } else {
        uf = 0.000001f;
      }  // assign low proba if word is not found
      wordDist += -log(uf);
    }

    float distanceI = lengthDist + wordDist;
    return distanceI;
  } else if (y1 == 0)  // deletion
  {
    // compute length statistics
    float relfs =
        length_distributions.relative_frequencies_of_source.find(x1)->second;
    float lengthDist = -log(relfs) - log(prior10);

    vector<long int>::iterator itr_source_token;

    // compute unigram frequencies
    float uf = 0.0f;
    float wordDist = 0.0f;

    // iterate through source token and sum up neg log unigram frequencies
    for (itr_source_token = st.begin(); itr_source_token != st.end();
         itr_source_token++) {
      long int token = *itr_source_token;

      if (lexical_distributions.unigram_frequencies_of_source.find(token) !=
          lexical_distributions.unigram_frequencies_of_source.end()) {
        uf = lexical_distributions.unigram_frequencies_of_source.find(token)
                 ->second;
      } else {
        uf = 0.0000001f;
      }  // assign low proba if word is not found
      wordDist += -log(uf);
    }
    float distanceD = lengthDist + wordDist;
    return distanceD;
  } else  // substitution
  {
    // compute length statistics
    float relfs =
        length_distributions.relative_frequencies_of_source.find(x1)->second;
    float distance = length_distributions.distanceMeasurePoiss(x1, y1);
    float lengthDist = distance - log(relfs) - log(prior11);

    // iterators over token
    vector<long int>::iterator itr_source_token;
    vector<long int>::iterator itr_target_token;

    // compute unigram frequencies
    float uf = 0.0;
    float wordDist = 0.0f;

    // loop through source token and sum up neg liog unigram frequencies
    for (itr_source_token = st.begin(); itr_source_token != st.end();
         itr_source_token++) {
      long int token = *itr_source_token;

      if (lexical_distributions.unigram_frequencies_of_source.find(token) !=
          lexical_distributions.unigram_frequencies_of_source.end()) {
        uf = lexical_distributions.unigram_frequencies_of_source.find(token)
                 ->second;
      } else {
        uf = 0.000001f;
      }  // assign low proba if word is not found
      wordDist += -log(uf);
    }

    // put null word at the end of the source token
    st.push_back(0);

    // trans proba of target given source
//    float transProb = 0.0f;

    // total translation probabilites for source words
    float totalTPST = 0.0f;

    // maximum translation probability for source words
//    float maxTP = -log(0.0f);

    // compute model 1 score
    // loop through token of target sentence
    vector<double> max_probs(y1, 0.0);

	for (const auto stok : st) {
		const vector<float> probs = lexical_distributions.getTTableEntries(stok, tt);
		for (int i=0; i<y1; ++i) {
			if (probs[i] > max_probs[i]) {
				max_probs[i] = probs[i];
			}
		}
	}
	for ( int i=0; i<y1; ++i) {
		totalTPST -= log(max_probs[i]);
	}

//    for (const auto ttok : tt) {
//    	double maxSourceProb = 0;
//    		const double prob = lexical_distributions.getTTableEntry(stok, ttok);
//    		if (prob > maxSourceProb) {
//    			maxSourceProb = prob;
//    		}
//    	}
//    	totalTPST -= log(maxSourceProb);
//    }


//    const auto lex_0 = lexical_distributions.translation_table.find(0);
//    for (itr_target_token = tt.begin(); itr_target_token != tt.end();
//         itr_target_token++) {
//      long int ttok = *itr_target_token;
//      maxTP = -log(0.0f);
//
//      // loop through token of source sentence
//      for (itr_source_token = st.begin(); itr_source_token != st.end();
//           itr_source_token++) {
//        long int stok = *itr_source_token;
//
//        // check if words are in translation table
//        // look for source word
//        const auto s_lex = lexical_distributions.translation_table.find(stok);
//        if (s_lex != lexical_distributions.translation_table.end()) {
//          // if source word is found, look for associated target word
//          auto t_lex = s_lex->second.find(ttok);
//          if (t_lex != s_lex->second.end()) {
//            // if found get neg log probability
//            transProb = -log(t_lex->second);
//            // take the maximum (Viterbi Model 1)
//            if (transProb <= maxTP) {
//              maxTP = transProb;
//            }
//          }
//          // else search for target token associated to null word (as source
//          // word)
//          else {
//            if (lex_0->second.find(ttok) != lex_0->second.end()) {
//              // if found get neg log probability
//              transProb = -log(lex_0->second.find(ttok)->second);
//              // take the maximum (Viterbi Model 1)
//              if (transProb <= maxTP) {
//                maxTP = transProb;
//              }
//            } else {
//              transProb =
//                  -log(0.0000001f);  // assign low proba if word is not found
//              if (transProb <= maxTP) {
//                maxTP = transProb;
//              }
//            }
//          }
//        }  // end of if
//        // else look for the null word
//        else {
//          if (lex_0->second.find(ttok) !=
//              lexical_distributions.translation_table.find(0)->second.end()) {
//            transProb = -log(lexical_distributions.translation_table.find(0)
//                                 ->second.find(ttok)
//                                 ->second);
//            if (transProb <= maxTP) {
//              maxTP = transProb;
//            }
//          } else {
//            transProb =
//                -log(0.0000001f);  // assign low proba if word is not found
//            if (transProb <= maxTP) {
//              maxTP = transProb;
//            }
//          }
//        }
//      }  // end of loop through source token
//      totalTPST += maxTP;
//    }  // end of loop through target tok

    // compute Viterbi model-1 probability
    float model_1 = totalTPST + log(((float)st.size() + 1.0f));
    float end_distance = lengthDist + model_1 + wordDist;
    return end_distance;
  }  // end of else
  return -1;
}

// align : reiterate dynamic programming with incrementing bandwidth until the
// whole text has been aligned
void InitialAlignment::align(Data &my_data, const LengthDistributions &l_d,
                             const LexicalDistributions &lex_d) {
  // Get source and target lengths to size the distance matrix
  int ssize = my_data.getSourceSentences().size();
  int tsize = my_data.getTargetSentences().size();

  setExpandBand1(0);
  setExpandBand2(0);

  // Handle empty sentences and paragraphs containing only 1 sentence-pair
  // if no sentences to align, do not align (empty sentences result from
  // sentences that do not contain at least a word)
  if (ssize == 0 && tsize == 0) {
    return;
  }
  // if only one sentence pair : only 1-to-1 alignment is possible
  if (ssize == 1 && tsize == 1) {
    // make alignment
    stack<Chunk> one_to_one_alignment;

    // set chunks
    vector<long int> source_position;
    source_position.push_back(
        my_data.getSourceSentences().front().getPosition());

    vector<long int> target_position;
    target_position.push_back(
        my_data.getTargetSentences().front().getPosition());

    vector<vector<long int> > source_chunk_token;
    source_chunk_token.push_back(
        my_data.getSourceSentences().front().getTokenInts());

    vector<vector<long int> > target_chunk_token;
    target_chunk_token.push_back(
        my_data.getTargetSentences().front().getTokenInts());

    Chunk my_chunk(source_position, target_position, source_chunk_token,
                   target_chunk_token);
    one_to_one_alignment.push(my_chunk);

    // set alignment
    setAlignment(one_to_one_alignment);
    return;
  }

  // incrementat the bandwidth by 1.5 at each iteration
  float increment_ratio = 1.5;

  int bandwidth_1 = abs(ssize - tsize) * increment_ratio;
  int bandwidth_2 = abs(ssize - tsize) * increment_ratio;

  // minimal size of bandwidth is 20
  if (bandwidth_1 < 20) {
    bandwidth_1 = 20;
  }

  if (bandwidth_2 < 20) {
    bandwidth_2 = 20;
  }

  /*
  Band expansions and reductions: if the alignment comes too close to the
  boundaries, expand
                                  if the alignment is far away enough from the
  boundaries, reduce
  */

  // initialize field indicating that the dynamic programming procedure is done
  setDynamicProgrammingState(0);
  bool dynamic_programming_done = 0;

  while (dynamic_programming_done != 1) {
    bool expand_band1 = getExpandBand1();
    bool expand_band2 = getExpandBand2();

    if (expand_band1 == 1) {
      bandwidth_1 = increment_ratio * bandwidth_1;
      expand_band1 = 0;
    }

    if (expand_band2 == 1) {
      bandwidth_2 = increment_ratio * bandwidth_2;
      expand_band2 = 0;
    }
    dynamicProgramming(my_data, l_d, lex_d, bandwidth_1, bandwidth_2);
    dynamic_programming_done = getDynamicPrgrammingState();
  }  // end of while
}

// dynamic programming : makes an approximate alignment on a narrow band around
// the flat diagonal extracted from the length-based alignment. If the alignment
// comes close to the bounds reiterate and replace the flat diagonal by the
// approximate alignment.
void InitialAlignment::dynamicProgramming(Data &my_data,
                                          const LengthDistributions &l_d,
                                          const LexicalDistributions &lex_d,
                                          int bandwidth1, int bandwidth2) {
  const vector<Sentence> &source_sentences = my_data.getSourceSentences();
  const vector<Sentence> &target_sentences = my_data.getTargetSentences();

  stack<Chunk> alignment_temp;

  // Get source and target lengths to size the distance matrix
  int ssize = source_sentences.size();
  int tsize = target_sentences.size();

  // initialize lower limit
  int lower_limit_row = 0;
  int upper_limit_row = bandwidth2;

  // distance matrix
  vector<vector<Cell> > distanceMatrix;

  // define max distance
  const float MAXD = numeric_limits<float>::infinity();

  // initial values for cells of distance matrix
  const float NullDistance = 0.0f;
  const float MaxDistance = MAXD;
  const pair<int, int> NullPrevious(-1, -1);

  // to store found traceback
  stack<pair<int, int> > trace_back;

  // Initialize row and columns of distance matrix
  int i = 0;
  int j = 0;

  // initial values of distance function variables
  float dSubst = 0.0;
  float dDelet = 0.0;
  float dInsert = 0.0;

  // null vector to fill up arguments of distance function
  vector<long int> nullV;

  // get diagonal for determining path through distance matrix
  list<pair<int, int> > diag_for_path;

  // initialize distance matrix : set all cells to MAXD
  distanceMatrix = vector<vector<Cell> >(
      ssize + 1,
      vector<Cell>(
          tsize + 1,
          Cell(MaxDistance,
               NullPrevious)));  // first how many vectors in the vector

  // for debugging
  // cout << "NUMBER OF SOURCE SENTENCES " << ssize << endl;
  // cout << "NUMBER OF TARGET SENTENCES " << tsize << endl;
  // cout << "Size of diagonal " << diagonal.size() << endl;

  // get the diagonal to determine the search space
  diag_for_path = getDiagonal();

  while (j <= tsize) {
    // for debugging
    // cout << "PROCESSING TARGET SENTENCE " <<  j << endl;

    if (upper_limit_row <= ssize) {
      upper_limit_row = diag_for_path.front().first + bandwidth2;
    } else {
      upper_limit_row = ssize;
    }
    if (diag_for_path.front().first - bandwidth1 >= 0) {
      lower_limit_row = diag_for_path.front().first - bandwidth1;
    } else {
      lower_limit_row = 0;
    }

    i = lower_limit_row;

    while (i >= lower_limit_row && i <= upper_limit_row && i <= ssize) {
      // cout << "PROCESSING SOURCE SENTENCE" <<  i << endl;

      if (i == 0 && j == 0) {
        // set first cell to 0 distance
        distanceMatrix.at(i).at(j).setDistance(NullDistance);

        // first cell is root of traceback
        pair<int, int> root_cell(i, j);
        trace_back.push(root_cell);

      }  // enf of if

      else {
        if (i > 0 && j > 0)  // substitution
        {
          float previousD = 0.0;
          previousD = distanceMatrix.at(i - 1).at(j - 1).getDistance();

          vector<long int> st;
          vector<long int> tt;

          st = source_sentences.at(i - 1).getTokenInts();
          tt = target_sentences.at(j - 1).getTokenInts();

          dSubst = previousD + distanceFunctionMoore(l_d, lex_d, st, tt);

        }  // end of if
        else {
          dSubst = MAXD;
        }

        if (i > 0)  // deletion
        {
          float previousD = 0.0f;
          previousD = distanceMatrix.at(i - 1).at(j).getDistance();

          vector<long int> st;

          st = source_sentences.at(i - 1).getTokenInts();

          dDelet = previousD + distanceFunctionMoore(l_d, lex_d, st, nullV);

        }  // end of if
        else {
          dDelet = MAXD;
        }

        if (j > 0)  // insertion
        {
          float previousD = 0.0f;
          previousD = distanceMatrix.at(i).at(j - 1).getDistance();

          vector<long int> tt;
          tt = target_sentences.at(j - 1).getTokenInts();

          dInsert = previousD + distanceFunctionMoore(l_d, lex_d, nullV, tt);
        }  // end of if
        else {
          dInsert = MAXD;
        }

        // look for minimal distance
        float dMin = MAXD;

        if (dSubst < dMin) {
          dMin = dSubst;
        }

        if (dDelet < dMin) {
          dMin = dDelet;
        }

        if (dInsert < dMin) {
          dMin = dInsert;
        }

        // set pointer to previous cell
        if (dMin == MAXD) {
          distanceMatrix.at(i).at(j).setDistance(MAXD);
        }

        else if (dMin == dSubst) {
          distanceMatrix.at(i).at(j).setDistance(dSubst);
          pair<int, int> previous;
          previous.first = i - 1;
          previous.second = j - 1;
          distanceMatrix.at(i).at(j).setPrevious(previous);
        }

        else if (dMin == dDelet) {
          distanceMatrix.at(i).at(j).setDistance(dDelet);
          pair<int, int> previous;
          previous.first = i - 1;
          previous.second = j;
          distanceMatrix.at(i).at(j).setPrevious(previous);
        }

        else if (dMin == dInsert) {
          distanceMatrix.at(i).at(j).setDistance(dInsert);
          pair<int, int> previous;
          previous.first = i;
          previous.second = j - 1;
          distanceMatrix.at(i).at(j).setPrevious(previous);
        }
      }  // end of first else (j != and i!= 0)

      // for debugging
      // cout << "CELL " << i << " " << j << endl;
      // cout << "DISTANCE MATRIX :" << distanceMatrix.at(i).at(j).getDistance()
      // << endl;
      // cout << "DISTANCE MATRIX :" <<
      // distanceMatrix.at(i).at(j).getPrevious().first << " " <<
      // distanceMatrix.at(i).at(j).getPrevious().first << endl;

      i++;
    }  // end of loop for i
    if (diag_for_path.size() > 1) {
      diag_for_path.pop_front();
    }
    j++;
  }  // end of first for
  // end of construction of the distance matrix

  // Trace back the rest of the distance matrix
  // begin at last cell
  pair<int, int> current_cell(ssize, tsize);

  // store found cells in BackTrack temp
  list<pair<int, int> > first_trace_back;
  stack<pair<int, int> > back_trace_temp;

  // trace until root cell is found
  pair<int, int> root_cell(0, 0);

  // make first complete alignment : if no alignment_temp is possible beause the
  // initial band is too narrow retiterate
  while (!(current_cell.first == root_cell.first &&
           current_cell.second == root_cell.second)) {
    first_trace_back.push_front(current_cell);
    back_trace_temp.push(current_cell);
    pair<int, int> previous_cell;

    previous_cell = distanceMatrix.at(current_cell.first)
                        .at(current_cell.second)
                        .getPrevious();

    if (previous_cell.first == -1 && previous_cell.second == -1) {
      if ((bandwidth1 <= ssize && bandwidth2 <= ssize) ||
          ssize < 2)  // if bandwidth < nbr of columns in the distance matrix
                      // try to align again by expanding the band
      {
        setExpandBand1(1);
        setExpandBand2(1);

        distanceMatrix.clear();
        setDynamicProgrammingState(0);

        cout << "Redo dynamic programming " << endl;
        return;
      } else  // if band is completely expanded a problem occured in the
              // construction of the distance matrix
      {
        cout
            << "No path has been found in DP procedure. Return empty alignment."
            << endl;
        distanceMatrix.clear();
        setDynamicProgrammingState(1);
        stack<Chunk> empty_alignment;
        setAlignment(empty_alignment);
        return;
      }
    }
    // set current cell to previous cell
    current_cell = previous_cell;
  }

  // check if the first alignment commes close to the flattened diagonal from
  // the first alignment

  // Add found cells to partial traceback
  while (!back_trace_temp.empty()) {
    current_cell = back_trace_temp.top();

    // get positions of cell in traceback
    int current_row = current_cell.first;

    pair<int, int> diagonal_cell = diagonal.front();  // direct access to
                                                      // diagonal : decremented
                                                      // as traceback proceeds

    int diag_row = diagonal_cell.first;

    if (diagonal.size() > 1) {
      diagonal.pop_front();
    }

    int boundary_1 = diag_row - bandwidth1 + abs(ssize - tsize);
    int boundary_2 = diag_row + bandwidth2 - abs(ssize - tsize);

    // if alignment comes close to band boundaries, redo dynamic programming
    // with extended bandwidth but begin
    if (current_row <= boundary_1) {
      setExpandBand1(1);

      // set new diagonal to found traceback
      setDiagonal(first_trace_back);

      distanceMatrix.clear();
      setDynamicProgrammingState(0);
      return;
    } else if (current_row >= boundary_2) {
      setExpandBand2(1);

      // set new diagonal to found traceback
      setDiagonal(first_trace_back);

      distanceMatrix.clear();
      setDynamicProgrammingState(0);
      return;
    } else {
      back_trace_temp.pop();
      trace_back.push(current_cell);
    }
  }  // end of while
  // end of traceback

  // find and display chunks (offset positions + sentences) corresponding to
  // cells in traceback

  // initilize chunks
  vector<long int> source_chunk_position;
  vector<long int> target_chunk_position;

  // emtpy token vectors
  vector<vector<long int> > source_chunk_token;
  vector<vector<long int> > target_chunk_token;

  Chunk my_chunk = Chunk(source_chunk_position, target_chunk_position,
                         source_chunk_token, target_chunk_token);

  // set current cell to first cell on the traceback stack
  pair<int, int> last_cell = trace_back.top();

  // loop through until root cell is reached
  while (!(last_cell.first == root_cell.first &&
           last_cell.second == root_cell.second)) {
    // write cells in traceback log file
    trace_back.pop();

    // get previous cell
    pair<int, int> previous_cell = trace_back.top();

    // what type of correspondance
    // check for substitution (1-to-1)
    if ((previous_cell.first == last_cell.first - 1) &&
        (previous_cell.second == last_cell.second - 1)) {
      // extract sentence positions and token
      long int source_position =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token =
          source_sentences[last_cell.first - 1].getTokenInts();

      // extract sentence positions and token
      long int target_position =
          target_sentences[last_cell.second - 1].getPosition();
      vector<long int> target_token =
          target_sentences[last_cell.second - 1].getTokenInts();

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToSourceChunkToken(source_token);
      my_chunk.addToTargetChunkToken(target_token);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // check for deletion
    if ((previous_cell.first == last_cell.first - 1) &&
        (previous_cell.second == last_cell.second)) {
      // extract sentence positions and token
      long int source_position =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token =
          source_sentences[last_cell.first - 1].getTokenInts();

      long int target_position = -1;

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToSourceChunkToken(source_token);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // check for insertion
    else if ((previous_cell.first == last_cell.first) &&
             (previous_cell.second == last_cell.second - 1)) {
      // extract sentence positions and token
      long int source_position = -1;
      long int target_position =
          target_sentences[last_cell.second - 1].getPosition();
      vector<long int> target_token =
          target_sentences[last_cell.second - 1].getTokenInts();

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToTargetChunkToken(target_token);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // set current cell to previous cell
    last_cell = previous_cell;
  }  // end of while
  distanceMatrix.clear();
  setAlignment(alignment_temp);
  // clear alignment temp
  while (!alignment_temp.empty()) {
    alignment_temp.pop();
  }
  setDynamicProgrammingState(1);
  return;
}
