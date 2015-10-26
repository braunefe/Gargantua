#ifndef SERIALIZERS_H_
#define SERIALIZERS_H_

#include <iostream>
#include <fstream>
#include "boost/serialization/map.hpp"
#include "boost/serialization/string.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#include "LengthDistributions.h"
#include "Aligner.h"

namespace bfs = boost::filesystem;
namespace ba = boost::archive;
using namespace std;

class Serializers {
 public:
  // serializes length distributions
  static void serializeLengthDistributions(
      const LengthDistributions &my_length_distributions);

  // restores length distributions
  static LengthDistributions restoreLengthDistributions();

  // serializes flat diagonals
  static void serializeFlatDiagonals(const Aligner &my_aligner_for_diagonal,
                                     const string &path_to_serialized,
                                     const string &filename);

  // restores flat diagonals
  static Aligner restoreFlatDiagonals(const string &path_to_serialized,
                                      const string &filename);
};

#endif /*UTILITIES_H_*/
