#include <string>
#include <vector>
#include "Sentence.h"

using namespace std;

Sentence::Sentence(long int position, int charlength, int wordlength,
                   const vector<long int>& token) {
  setSentence(position, charlength, wordlength, token);
}

void Sentence::setSentence(long int position, int charlength, int wordlength,
                           const vector<long int>& t) {
  _sPosition = position;
  _sLengthChar = charlength;
  _sLengthWords = wordlength;
  _token_ints = t;
}

long int Sentence::getPosition() const { return _sPosition; }

void Sentence::setPosition(long int p) { _sPosition = p; }

int Sentence::getCharLength() const { return _sLengthChar; }

void Sentence::setCharLength(int l) { _sLengthChar = l; }

int Sentence::getWordLength() const { return _sLengthWords; }

void Sentence::setWordLength(int l) { _sLengthWords = l; }

vector<string> Sentence::getToken() const { return _token; }

void Sentence::setToken(const vector<string>& t) { _token = t; }

void Sentence::setTokenInts(const vector<long int>& t_i) { _token_ints = t_i; }

vector<long int> Sentence::getTokenInts() const { return _token_ints; }
