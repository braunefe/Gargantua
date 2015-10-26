#ifndef DATA_H_
#define DATA_H_

#include "Sentence.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "boost/regex.hpp"
#include "boost/algorithm/string/regex.hpp"
#include <sstream>

using namespace boost;
using namespace std;

class Data {
 private:
  string corpus_name;
  vector<Sentence> source_sentences;
  vector<Sentence> target_sentences;

 public:
  Data(){};
  Data(const string& corpus_name, const vector<Sentence>& source_sentences,
		  const vector<Sentence>& target_sentences);

  string getCorpusName() const;
  void setCorpusName(const string& corpus_name);

//  string getParagraphName() const;
//  void setParagraphName(string par_name);

  vector<Sentence> getSourceSentences() const;
  vector<Sentence> getTargetSentences() const;

  void setSourceSentences(const vector<Sentence>& s);
  void setTargetSentences(const vector<Sentence>& t);

  void clear();
};

#endif /*DATA_H_*/
