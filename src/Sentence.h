#include <string>
#include <vector>

#ifndef SENTENCE_H_
#define SENTENCE_H_

using namespace std;

class Sentence {
 private:
  long int _sPosition;    // position of the sentence in the corpus
  int _sLengthChar;       // length of the sentence in characters
  int _sLengthWords;      // length of the sentence in words
  vector<string> _token;  // token composing the sentence
  vector<long int> _token_ints;  // sentence tokens as long ints

 public:
  Sentence(long int sPosition, int sLengthChar, int sLengthWords,
           const vector<long int>& token_ints);

  void setSentence(long int position, int charLength, int wordLength,
                   const vector<long int>& token_ints);

  void setPosition(long int p);
  long int getPosition() const;

  void setCharLength(int l);
  int getCharLength() const;

  void setWordLength(int w);
  int getWordLength() const;

  void setToken(const vector<string>& t);
  vector<string> getToken() const;

  void setTokenInts(const vector<long int>& t);
  vector<long int> getTokenInts() const;
};

#endif /*SENTENCE_H_*/
