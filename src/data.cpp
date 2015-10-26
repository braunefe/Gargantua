#include "Data.h"

Data::Data(const string& c_n, const vector<Sentence>& source_sent,
           const vector<Sentence>& target_sent) {
  setCorpusName(c_n);
  setSourceSentences(source_sent);
  setTargetSentences(target_sent);
}

string Data::getCorpusName() const { return corpus_name; }

void Data::setCorpusName(const string& c_n) { corpus_name = c_n; }

vector<Sentence> Data::getSourceSentences() const { return source_sentences; }

vector<Sentence> Data::getTargetSentences() const { return target_sentences; }

void Data::setSourceSentences(const vector<Sentence>& s) { source_sentences = s; }

void Data::setTargetSentences(const vector<Sentence>& t) { target_sentences = t; }

void Data::clear() {
  source_sentences.clear();
  target_sentences.clear();
  corpus_name.clear();
}
