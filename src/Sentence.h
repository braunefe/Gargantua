#include <string>
#include <vector>

#ifndef SENTENCE_H_
#define SENTENCE_H_

using namespace std;

class Sentence {
 private:
  long int sPosition;    // position of the sentence in the corpus
  int sLengthChar;       // length of the sentence in characters
  int sLengthWords;      // length of the sentence in words
  vector<string> token;  // token composing the sentence
  vector<long int>
      token_ints;  // long ints corresponding to token composing the sentence

 public:
  Sentence(long int sPosition, int sLengthChar, int sLengthWords,
           vector<long int> token_ints);

  void setSentence(long int position, int charLength, int wordLength,
                   vector<long int> token_ints);

  void setPosition(long int p);
  long int getPosition() const;

  void setCharLength(int l);
  int getCharLength() const;

  void setWordLength(int w);
  int getWordLength() const;

  void setToken(vector<string> t);
  vector<string> getToken() const;

  void setTokenInts(vector<long int> t);
  vector<long int> getTokenInts() const;
};

#endif /*SENTENCE_H_*/
