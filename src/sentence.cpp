#include <string>
#include <vector>
#include "Sentence.h"

using namespace std;

Sentence::Sentence(long int position, int charlength, int wordlength,
                   vector<long int> token) {
  setSentence(position, charlength, wordlength, token);
}

void Sentence::setSentence(long int position, int charlength, int wordlength,
                           vector<long int> t) {
  sPosition = position;
  sLengthChar = charlength;
  sLengthWords = wordlength;
  token_ints = t;
}

long int Sentence::getPosition() const { return sPosition; }

void Sentence::setPosition(long int p) { sPosition = p; }

int Sentence::getCharLength() const { return sLengthChar; }

void Sentence::setCharLength(int l) { sLengthChar = l; }

int Sentence::getWordLength() const { return sLengthWords; }

void Sentence::setWordLength(int l) { sLengthWords = l; }

vector<string> Sentence::getToken() const { return token; }

void Sentence::setToken(vector<string> t) { token = t; }

void Sentence::setTokenInts(vector<long int> t_i) { token_ints = t_i; }

vector<long int> Sentence::getTokenInts() const { return token_ints; }
