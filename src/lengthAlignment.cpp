#include "LengthAlignment.h"

stack<pair<int, int> > LengthAlignment::getTraceBack() { return trace_back; }

void LengthAlignment::setTraceBack(stack<pair<int, int> > t) { trace_back = t; }

stack<Chunk> LengthAlignment::getAlignment() { return alignment; }

void LengthAlignment::setAlignment(stack<Chunk> a) { alignment = a; }

// sentence length based distance function as defined in Moore 2002
float LengthAlignment::distanceFunctionMoore(
    const LengthDistributions& length_distributions, int x1, int y1, int x2,
    int y2) {
  // prior table of Moore
  const float prior11 = 0.94;
  const float prior12 = 0.02;
  const float prior21 = 0.02;
  const float prior10 = 0.01;
  const float prior01 = 0.01;

  if (x2 == 0 && y2 == 0) {
    if (x1 == 0)  // insertion
    {
      const float relft =
          length_distributions.relative_frequencies_of_target.find(y1)->second;
      return -log(relft) - log(prior01);
    }

    else if (y1 == 0)  // deletion
    {
      const float relfs =
          length_distributions.relative_frequencies_of_source.find(x1)->second;
      return -log(relfs) - log(prior10);
    }

    else  // substitution

    {
      const float relfs =
          length_distributions.relative_frequencies_of_source.find(x1)->second;
      const float distanceMeasure =
          length_distributions.distanceMeasurePoiss(x1, y1);
      return distanceMeasure - log(relfs) - log(prior11);
    }

  }

  else if (x2 == 0)  // expansion
  {
    const float relfs =
        length_distributions.relative_frequencies_of_source.find(x1)->second;
    const float distanceMeasure =
        length_distributions.distanceMeasurePoiss(x1, y1 + y2);
    return distanceMeasure - log(relfs) - log(prior12);
  }

  else if (y2 == 0)  // contraction
  {
    const float relfs1 =
        length_distributions.relative_frequencies_of_source.find(x1)->second;
    const float relfs2 =
        length_distributions.relative_frequencies_of_source.find(x2)->second;
    const float distanceMeasure =
        length_distributions.distanceMeasurePoiss(x1 + x2, y1);
    return distanceMeasure - log(relfs1) - log(relfs2) - log(prior21);
  }
  return -1;
}

// dynamic Programming procedure : searches a narrow band around the main
// diagonal (i=j). If the search comes close to the bound, the procedure
// reiterates
void LengthAlignment::dynamicProgramming(
    const Data& my_data, const LengthDistributions& l_d,
    //                                         bool expand_band,
    int bandwidth) {
  const float MAXD = numeric_limits<float>::infinity();

  // intitialize disance variables
  float dSubst = 0.0;
  float dDelet = 0.0;
  float dInsert = 0.0;
  float dContr = 0.0;
  float dExp = 0.0;

  // found trace-back : necessary in order to compute the flat diagonal
  stack<pair<int, int> > trace_back;

  // found alignment
  stack<Chunk> alignment_temp;

  const vector<Sentence>& source_sentences = my_data.getSourceSentences();
  const vector<Sentence>& target_sentences = my_data.getTargetSentences();

  // DistanceMatrix declaration
  // Get source and target lengths to size the distance matrix
  int ssize = source_sentences.size();
  int tsize = target_sentences.size();

  // distance Matrix
  vector<vector<Cell> > distanceMatrix;

  // initial values for cells
  const float NullDistance = 0.0;
  const float MaxDistance = MAXD;
  const pair<int, int> NullPrevious(-1, -1);

  // initialization of row search
  int init = 0;

  if (bandwidth == 0) {
    bandwidth = abs(ssize - tsize);  // bandwidth : at least the absolute
                                     // difference between number of source and
                                     // target sentences
  }
  if (bandwidth < 1) {  // minimal widthdraw of 1 (Europarl paragraphs tend to
                        // be very small)
    bandwidth = 2;
  }

  //  if (expand_band == 0) {
  //    bandwidth = abs(ssize - tsize);  // bandwidth : at least the absolute
  //                                     // difference between number of source
  //                                     and
  //                                     // target sentences
  //
  //    if (bandwidth < 1)  // minimal widthdraw of 1 (Europarl paragraphs tend
  //    to
  //                        // be very small)
  //    {
  //      bandwidth = 2;
  //    }
  //  }
  //
  //  // if the search comes to the bounds of the band, expand by 0.5 *
  //  bandwidth
  //  if (expand_band == 1) {
  //    bandwidth += bandwidth;
  //    expand_band = 0;
  //
  //    // cout << "BAND EXPANSION" << endl;
  //    // cout << "bandwidth " << bandwidth << endl;
  //  }

  int offset = ((abs(ssize - tsize)) / 25) + 1;  // offset determining how close
                                                 // to the bounds the
                                                 // approximate alignment is
                                                 // allowed to come

  // set lower and upper limits for search in rows
  int lower_limit_row = 0;
  int upper_limit_row = bandwidth;

  // Dynamic Programming search
  // start at Cell(0,0)
  int j = 0;
  int i = 0;

  // initialize distance matrix : set all cells to MAXD
  distanceMatrix = vector<vector<Cell> >(
      ssize + 1, vector<Cell>(tsize + 1, Cell(MaxDistance, NullPrevious)));
  // first how many vectors in the vector

  while (j <= tsize) {  // for debugging
    // cout << "processing target " << j << endl;
    i = init;  // if column bigger than bandwidth is reached, increment lower
               // limit

    while (i >= lower_limit_row && i <= upper_limit_row && i <= ssize) {
      // for debugging
      // cout << "processing source " << i << endl;
      if (i == 0 && j == 0) {
        // set first cell to 0 distance
        distanceMatrix.at(0).at(0).setDistance(NullDistance);

        // first cell is root of traceback
        pair<int, int> root_cell(i, j);
        trace_back.push(root_cell);
      }  // enf of if

      else  // for i > 0
      {
        if (i > 0 && j > 0) /*substitution*/
        {
          // for debugging
          // cout << "in subst " << i << j << endl;

          const float previousD =
              distanceMatrix.at(i - 1).at(j - 1).getDistance();

          // for debugging
          // cout << "previous subst " << previousD << endl;
          // cout << "indexes " << i << " : " << j << endl;
          // cout << "distance subst " << dSubst << endl;

          const int l1 = source_sentences.at(i - 1).getWordLength();
          const int l2 = target_sentences.at(j - 1).getWordLength();
          dSubst = previousD + distanceFunctionMoore(l_d, l1, l2, 0, 0);

        }  // end of if
        else {
          dSubst = MAXD;
        }

        if (i > 0) /*deletion*/
        {
          // for debugging
          // cout << "in del " << i << j << endl;

          const float previousD = distanceMatrix.at(i - 1).at(j).getDistance();

          // for debugging
          // cout << "previous delet " << previousD << endl;
          // cout << "indexes " << i << " : " << j << endl;
          // cout << "distance delet " << dDelet << endl;

          const int l1 = source_sentences.at(i - 1).getWordLength();
          const int l2 = 0;
          dDelet = previousD + distanceFunctionMoore(l_d, l1, l2, 0, 0);

        }  // end of if
        else {
          dDelet = MAXD;
        }

        if (j > 0) /*insertion*/
        {
          // for debugging
          // cout << "in insert " << i << j << endl;

          const float previousD = distanceMatrix.at(i).at(j - 1).getDistance();

          // for debugging
          // cout << "previous insert " << previousD << endl;
          // cout << "indexes " << i << " : " << j << endl;
          // cout << "distance insert " << dInsert << endl;

          const int l1 = 0;
          const int l2 = target_sentences.at(j - 1).getWordLength();
          dInsert = previousD + distanceFunctionMoore(l_d, l1, l2, 0, 0);

        }  // end of if
        else {
          dInsert = MAXD;
        }

        if (i > 1 && j > 0) /*contraction*/
        {
          // for debugging
          // cout << "in contr " << i << j << endl;

          const float previousD =
              distanceMatrix.at(i - 2).at(j - 1).getDistance();

          // for debugging
          // cout << "previous contr " << previousD << endl;
          // cout << "indexes " << i << " : " << j << endl;
          // cout << "distance contr " << dContr << endl;

          const int l1 = source_sentences.at(i - 2).getWordLength();
          const int l2 = target_sentences.at(j - 1).getWordLength();
          const int l3 = source_sentences.at(i - 1).getWordLength();
          dContr = previousD + distanceFunctionMoore(l_d, l1, l2, l3, 0);

        }  // end of if
        else {
          dContr = MAXD;
        }

        if (i > 0 && j > 1) /*expansion*/
        {
          // for debugging
          // cout << "in exp " << i << j << endl;

          const float previousD =
              distanceMatrix.at(i - 1).at(j - 2).getDistance();

          // for debugging
          // cout << "previous exp " << previousD << endl;
          // cout << "indexes " << i << " : " << j << endl;
          // cout << "distance exp " << dExp << endl;

          const int l1 = source_sentences.at(i - 1).getWordLength();
          const int l2 = target_sentences.at(j - 2).getWordLength();
          const int l3 = target_sentences.at(j - 1).getWordLength();
          dExp = previousD + distanceFunctionMoore(l_d, l1, l2, 0, l3);

        }  // end of if
        else {
          dExp = MAXD;
        }

        // look for minimal distance
        float dMin = dSubst;

        if (dDelet < dMin) {
          dMin = dDelet;
        }

        if (dInsert < dMin) {
          dMin = dInsert;
        }

        if (dContr < dMin) {
          dMin = dContr;
        }

        if (dExp < dMin) {
          dMin = dExp;
        }

        // set pointer to previous cell
        if (dMin == MAXD) {
          distanceMatrix.at(i).at(j).setDistance(MAXD);
        }

        else if (dMin == dSubst) {
          // for debugging
          // cout << "min subst " << endl;

          distanceMatrix.at(i).at(j).setDistance(dSubst);
          pair<int, int> previous;
          previous.first = i - 1;
          previous.second = j - 1;
          distanceMatrix.at(i).at(j).setPrevious(previous);

        }

        else if (dMin == dDelet) {
          // for debugging
          // cout << "min delet " << endl;

          distanceMatrix.at(i).at(j).setDistance(dDelet);
          pair<int, int> previous;
          previous.first = i - 1;
          previous.second = j;
          distanceMatrix.at(i).at(j).setPrevious(previous);

        }

        else if (dMin == dInsert) {
          // for debugging
          // cout << "min insert " << endl;

          distanceMatrix.at(i).at(j).setDistance(dInsert);
          pair<int, int> previous;
          previous.first = i;
          previous.second = j - 1;
          distanceMatrix.at(i).at(j).setPrevious(previous);

        }

        else if (dMin == dContr) {
          // for debugging
          // cout << "min contr " << endl;

          distanceMatrix.at(i).at(j).setDistance(dContr);
          pair<int, int> previous;
          previous.first = i - 2;
          previous.second = j - 1;
          distanceMatrix.at(i).at(j).setPrevious(previous);

        } else if (dMin == dExp) {
          // for debugging
          // cout << "min exp " << endl;

          distanceMatrix.at(i).at(j).setDistance(dExp);
          pair<int, int> previous;
          previous.first = i - 1;
          previous.second = j - 2;
          distanceMatrix.at(i).at(j).setPrevious(previous);
        }

      }  // end of first else (j != and i!= 0)
      i++;
    }  // end of while for i
    upper_limit_row++;
    if (j >= bandwidth)  // if column bigger than bandwidth is reached,
                         // increment lower limit
    {
      lower_limit_row++;
      init++;
    }
    j++;
  }  // end of while for j
  // end of construction of the distance matrix

  // traceback

  // begin at last cells
  pair<int, int> current_cell(ssize, tsize);

  // store found cells in BackTrack temp
  stack<pair<int, int> > back_track_temp;

  // cells from the main diagonals
  pair<int, int> main_diagonal(ssize, tsize);

  // root cell
  pair<int, int> root_cell(0, 0);
  pair<int, int> previous_cell;

  // loop through cells and check if they come close to the external boundaries
  // of the fixed bandwith.
  while (!(current_cell.first == root_cell.first &&
           current_cell.second == root_cell.second)) {
    // get positions of cell in traceback
    const int current_row = current_cell.first;
    const int diag_row = main_diagonal.first;

    const int boundary_1 = diag_row - bandwidth + offset;
    const int boundary_2 = diag_row + bandwidth - offset;

    // for debugging
    // cout << "\nboundary1 " << boundary_1 << endl;
    // cout << "boundary2 " << boundary_2 << endl;

    // if alignment comes close to band boundaries, redo dynamic programming
    // with extended bandwidth
    if (bandwidth < ssize && bandwidth < tsize &&
        (current_row <= boundary_1 || current_row >= boundary_2 ||
         current_row == -1)) {
      //      expand_band = 1;

      // clear distanceMatrix and recurse with expanded band
      distanceMatrix.clear();
      cout << "Increasing bandwidth to " << 2 * bandwidth << endl;
      dynamicProgramming(my_data, l_d, bandwidth * 2);
      return;
    }

    else {
      // store in back track temp : trace_back needs to be reversed when
      // searching chunks corresponding to cells and for final alignment
      // diagonal
      back_track_temp.push(current_cell);

      // get previous cell
      previous_cell =
          distanceMatrix[current_cell.first][current_cell.second].getPrevious();

      // decrement main diagonal
      main_diagonal.first--;
      main_diagonal.second--;

      // set current cell to previous cell
      current_cell = previous_cell;
    }
  }

  // Reverse order of cells in trace_back
  while (!back_track_temp.empty()) {
    pair<int, int> cell = back_track_temp.top();
    trace_back.push(cell);
    back_track_temp.pop();
  }

  // Store trace_back : needed to compute the diagonal for the final alignment
  setTraceBack(trace_back);

  // initilize chunks
  vector<long int> source_chunk_position;
  vector<long int> target_chunk_position;
  vector<vector<long int> > source_chunk_token;
  vector<vector<long int> > target_chunk_token;

  // make empty chunk for insertions and deletions
  vector<long int> null_v;

  Chunk my_chunk = Chunk(source_chunk_position, target_chunk_position,
                         source_chunk_token, target_chunk_token);

  // set current cell to first cell on the traceback stack
  pair<int, int> last_cell = trace_back.top();

  // loop through the trace-back until root cell is reached
  while (!(last_cell.first == root_cell.first &&
           last_cell.second == root_cell.second)) {
    trace_back.pop();

    // get previous cell
    pair<int, int> previous_cell = trace_back.top();

    // what type of correspondance ?
    // check for substitution
    if ((previous_cell.first == last_cell.first - 1) &&
        (previous_cell.second == last_cell.second - 1)) {
      // extract sentence positions at position [i-1, j-1]
      long int source_position =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token =
          source_sentences[last_cell.first - 1].getTokenInts();

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
    else if ((previous_cell.first == last_cell.first - 1) &&
             (previous_cell.second == last_cell.second)) {
      // extract sentence positions at position [i-1, j-1]
      long int source_position =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token =
          source_sentences[last_cell.first - 1].getTokenInts();

      long int target_position = -1;

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToSourceChunkToken(source_token);
      my_chunk.addToTargetChunkToken(
          null_v);  // add empty vector to target token
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // check for insertion
    else if ((previous_cell.first == last_cell.first) &&
             (previous_cell.second == last_cell.second - 1)) {
      // extract sentence positions at position [i-1, j-1]
      long int target_position =
          target_sentences[last_cell.second - 1].getPosition();
      vector<long int> target_token =
          target_sentences[last_cell.second - 1].getTokenInts();

      long int source_position = -1;

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToSourceChunkToken(
          null_v);  // add empty vector to source chunk token
      my_chunk.addToTargetChunkToken(target_token);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // check for contraction
    else if ((previous_cell.first == last_cell.first - 2) &&
             (previous_cell.second == last_cell.second - 1)) {
      // extract sentence positions at position [i-2, j-1]
      long int source_position_1 =
          source_sentences[last_cell.first - 2].getPosition();
      vector<long int> source_token_1 =
          source_sentences[last_cell.first - 2].getTokenInts();

      long int source_position_2 =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token_2 =
          source_sentences[last_cell.first - 1].getTokenInts();

      long int target_position =
          target_sentences[last_cell.second - 1].getPosition();
      vector<long int> target_token =
          target_sentences[last_cell.second - 1].getTokenInts();

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position_1);
      my_chunk.addToSourceChunkToken(source_token_1);
      my_chunk.addToSourceChunkPosition(source_position_2);
      my_chunk.addToSourceChunkToken(source_token_2);
      my_chunk.addToTargetChunkPosition(target_position);
      my_chunk.addToTargetChunkToken(target_token);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    }

    // check for expansion
    else if ((previous_cell.first == last_cell.first - 1) &&
             (previous_cell.second == last_cell.second - 2)) {
      // extract sentence positions at position [i-1, j-2]
      long int source_position =
          source_sentences[last_cell.first - 1].getPosition();
      vector<long int> source_token =
          source_sentences[last_cell.first - 1].getTokenInts();

      long int target_position_1 =
          target_sentences[last_cell.second - 2].getPosition();
      vector<long int> target_token_1 =
          target_sentences[last_cell.second - 2].getTokenInts();

      long int target_position_2 =
          target_sentences[last_cell.second - 1].getPosition();
      vector<long int> target_token_2 =
          target_sentences[last_cell.second - 1].getTokenInts();

      // put positions in sentence chunk
      my_chunk.addToSourceChunkPosition(source_position);
      my_chunk.addToSourceChunkToken(source_token);
      my_chunk.addToTargetChunkPosition(target_position_1);
      my_chunk.addToTargetChunkToken(target_token_1);
      my_chunk.addToTargetChunkPosition(target_position_2);
      my_chunk.addToTargetChunkToken(target_token_2);
      alignment_temp.push(my_chunk);
      my_chunk.clear();
    } else {
      // for debugging
      // fout2 << "Should not come here ! " << endl;
    }

    // set current cell to previous cell
    last_cell = previous_cell;
  }  // end of while
  setAlignment(alignment_temp);
  return;
}

// filters out the best 1-to-1 correspondences from the sentence length based
// alignment
stack<Chunk> LengthAlignment::filterOutBestOneToOnes(
    const LengthDistributions& l_d, stack<Chunk> a) {
  // fix threshold
  float thresh = 3;  //= ln(0.05)

  // sentence lengths of beads
  int slength = 0;
  int tlength = 0;

  // copy alignment
  stack<Chunk> alignment = a;

  // stack to store the best 1-1 beads
  stack<Chunk> bestOnes;

  // loop through alignment and get best ones
  while (!alignment.empty()) {
    Chunk bead = alignment.top();
    alignment.pop();

    // initialize lengths
    slength = 0;
    tlength = 0;

    // take source and target chunks of alignment
    vector<long int> sourcePos = bead.getSourceChunkPosition();
    vector<long int> targetPos = bead.getTargetChunkPosition();

    // if the alignment is 1-1, the chunks have size 1
    if ((int)sourcePos.size() == 1 && (int)targetPos.size() == 1) {
      long int spos = sourcePos.front();

      long int tpos = targetPos.front();

      // if the alignment is 1-1 source and target positions do not have value
      // -1
      if (spos != -1 && tpos != -1) {
        // get token
        vector<vector<long int> > source_sentences = bead.getSourceChunkToken();
        vector<vector<long int> > target_sentences = bead.getTargetChunkToken();

        vector<vector<long int> >::iterator itr_sent;

        for (itr_sent = source_sentences.begin();
             itr_sent != source_sentences.end(); itr_sent++) {
          vector<long int> sentence = *itr_sent;
          slength += sentence.size();
        }

        for (itr_sent = target_sentences.begin();
             itr_sent != target_sentences.end(); itr_sent++) {
          vector<long int> sentence = *itr_sent;
          tlength += sentence.size();
        }

        // compute Moore's poisson distance function for 1-to-1 correspondence
        float match = 0.0;
        match = l_d.distanceMeasurePoiss(slength, tlength);

        if (match < thresh) {
          bestOnes.push(bead);
        }

      }  // end of first if
    }    // end of second if
  }      // end of while
  return bestOnes;
}
