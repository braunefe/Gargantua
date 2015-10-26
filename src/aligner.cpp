#include "Aligner.h"

using namespace std;

vector<list<pair<int, int> > > Aligner::getFlatDiagonals() {
  return flat_diagonals;
}
void Aligner::setFlatDiagonals(vector<list<pair<int, int> > > diag) {
  flat_diagonals = diag;
}

// makes sentence length alignment, returns correspondances with likelihood
// higer than 0.05 and traceback
pair<stack<Chunk>, vector<list<pair<int, int> > > >
Aligner::makeLengthBasedAlignment(stack<Data> &data,
                                  const LengthDistributions &l_d) {
  stack<Chunk> alignment;  // sentence length based alignment
  stack<Chunk> best_beads;  // beads with likelihood higher than threshold
  stack<Chunk> all_best_beads;  // beads with best scores in a document
  stack<Chunk> all_best_beads_reversed;  // put chunks in right order
  vector<list<pair<int, int> > >
      all_diagonals;  // vector to hold all flat diagonals
  stack<pair<int, int> > trace_back;
  list<pair<int, int> > flat_diagonal;
  list<pair<int, int> >::iterator itr_diag;
  pair<stack<Chunk>, vector<list<pair<int, int> > > >
      bests_and_diagonal;  // beads with best scores and flattened diagonal

  // for debugging
  // string path_to_log_file =
  // "./alignment_log_files/Tracebacks_form_length.txt";
  // ofstream fout(path_to_log_file.c_str());

  // string path_to_log_file_diag = "./alignment_log_files/FlatDiagonals.txt";
  // ofstream fout1(path_to_log_file_diag.c_str());

  while (!data.empty()) {
    Data d = data.top();
    data.pop();

    LengthAlignment my_length_alignment = LengthAlignment();
    my_length_alignment.dynamicProgramming(d, l_d, 0);
    alignment = my_length_alignment.getAlignment();
    best_beads = my_length_alignment.filterOutBestOneToOnes(l_d, alignment);
    trace_back = my_length_alignment.getTraceBack();
    stack<pair<int, int> > traceBack = my_length_alignment.getTraceBack();
    while (!traceBack.empty()) {
      pair<int, int> trace_cell = traceBack.top();
      // fout << trace_cell.first << " ; " << trace_cell.second << endl;
      traceBack.pop();
    }
    // fout << endl;

    flat_diagonal = flattenDiagonal(trace_back);
    for (itr_diag = flat_diagonal.begin(); itr_diag != flat_diagonal.end();
         itr_diag++) {
      pair<int, int> diag_cell = *itr_diag;
      // fout1 << diag_cell.first << " ; " << diag_cell.second << endl;
    }
    // fout1 << endl;

    while (!best_beads.empty()) {
      Chunk min_align = best_beads.top();
      all_best_beads.push(min_align);
      best_beads.pop();
    }

    // reverse stack
    while (!all_best_beads.empty()) {
      Chunk min_align = all_best_beads.top();
      all_best_beads_reversed.push(min_align);
      all_best_beads.pop();
    }
    all_diagonals.push_back(flat_diagonal);
  }

  bests_and_diagonal.first = all_best_beads_reversed;
  bests_and_diagonal.second = all_diagonals;

  return bests_and_diagonal;
}

// makes the alignment composed of 1-to-0/0-to-1 and 1-to-1 minimal alignments
stack<Chunk> Aligner::makeInitialAlignment(const vector<Data>& data,
                                           const LengthDistributions &l_d,
                                           const LexicalDistributions &lex_d,
                                           vector<list<pair<int, int> > > f_d) {
  // for debugging
  // cout << "Size of diagonal " << f_d.size() << endl;

  stack<Chunk> alignment;
  stack<Chunk> paragraph_alignment;

  for (const Data& d : data) {

    // iterator for vector of diagonals
    vector<list<pair<int, int> > >::iterator itr_diagonal;
    itr_diagonal = f_d.begin();

    list<pair<int, int> > diag = *itr_diagonal;
    list<pair<int, int> >::iterator itr_diag;

    // for debugging : go through diagonal and display each cell
    /*for(itr_diag = diag.begin(); itr_diag != diag.end(); itr_diag++)
    {
            pair<int, int> p = *itr_diag;
            //cout << "Diag cell " << p.first << ";" << p.second << endl;
    }*/

    InitialAlignment my_alignment = InitialAlignment(*itr_diagonal);
    f_d.erase(itr_diagonal);
    my_alignment.align(d, l_d, lex_d);
    paragraph_alignment = my_alignment.getAlignment();

    // put all paragraph alignments together
    while (!paragraph_alignment.empty()) {
      Chunk min_align = paragraph_alignment.top();
      alignment.push(min_align);
      paragraph_alignment.pop();
    }
  }
  return alignment;
}

// clusters the "initial alignment" (composed of 1-to-0/0-to-1 and 1-to-1
// alignments) to obtain the final alignment (up to 4 sentences on one side)
stack<Chunk> Aligner::makeAlignmentClustering(stack<Chunk> sentence_alignment,
                                              LengthDistributions l_d,
                                              LexicalDistributions lex_d) {
  if (sentence_alignment.empty()) {
    cout << "No alignment clustering possible. Empty alignment." << endl;
    return sentence_alignment;
  } else {
    stack<Chunk> clustered_alignment;

    AlignmentClustering my_alignment_clustering(sentence_alignment);
    my_alignment_clustering.estimatePriors(sentence_alignment);
    my_alignment_clustering.alignmentClustering(sentence_alignment, l_d, lex_d);
    clustered_alignment = my_alignment_clustering.getExpandedAlignment();

    return clustered_alignment;
  }
}

// flattens the diagonal from the initial alignment
list<pair<int, int> > Aligner::flattenDiagonal(
    stack<pair<int, int> > &traceback_from_initial_alignment) {
  // flattened diagonal
  list<pair<int, int> > flat_diagonal;

  vector<long int>::iterator itr_source_cells;
  vector<long int>::iterator itr_target_cells;

  // loop through traceback and reduce alignements to 1-to-1, 0-to-1 and 1-to-0
  pair<int, int> current_cell = traceback_from_initial_alignment.top();

  while (!traceback_from_initial_alignment.empty()) {
    traceback_from_initial_alignment.pop();

    pair<int, int> next_cell;
    if (!traceback_from_initial_alignment.empty()) {
      next_cell = traceback_from_initial_alignment.top();
    } else {
      next_cell.first = -1;
      next_cell.second = -1;
    }

    // check for 1-to-1 alignment (next = current -1 on both sides)
    if (next_cell.first == current_cell.first - 1 &&
        next_cell.second == current_cell.second - 1) {
      // do not modify
      flat_diagonal.push_front(current_cell);
    }

    // check for 1-to-0 alignment (next = current -1 on source side only)
    if (next_cell.first == current_cell.first - 1 &&
        next_cell.second == current_cell.second) {
      // do not modify
      flat_diagonal.push_front(current_cell);
    }

    // check for 0-to-1 alignment (next = current -1 on target side only)
    if (next_cell.first == current_cell.first &&
        next_cell.second == current_cell.second - 1) {
      // do not modify
      flat_diagonal.push_front(current_cell);
    }

    // check for 2-to-1 alignment (next = current -2 on source; next = current
    // -1 on target)
    if (next_cell.first == current_cell.first - 2 &&
        next_cell.second == current_cell.second - 1) {
      // decompose in 1-to-1 and 1-to-0
      // current cell is 1-to-1
      flat_diagonal.push_front(current_cell);

      // add 1-to-0
      pair<int, int> p1;
      p1.first = current_cell.first - 1;
      p1.second = current_cell.second;
      flat_diagonal.push_front(p1);
    }

    // check for 1-to-2 alignment (next = current -1 on source; next = current
    // -2 on target)
    if (next_cell.first == current_cell.first - 1 &&
        next_cell.second == current_cell.second - 2) {
      // decompose in 1-to-1 and and 0-to-1
      // current cell is 1-to-1
      flat_diagonal.push_front(current_cell);

      // add 0-to-1
      pair<int, int> p1;
      p1.first = current_cell.first;
      p1.second = current_cell.second - 1;
      flat_diagonal.push_front(p1);
    }

    current_cell = next_cell;
  }  // end of while

  return flat_diagonal;
}
