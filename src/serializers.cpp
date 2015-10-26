#include "Serializers.h"

// serializes length distributions
void Serializers::serializeLengthDistributions(
    const LengthDistributions &my_length_distributions) {
  // create and open a character archive for output
  std::ofstream ofs1("LengthDistributions");

  // save data to archive
  {
    ba::text_oarchive oa(ofs1);
    // write class instance to archive
    oa << my_length_distributions;
  }
}

// restores length distributions
LengthDistributions Serializers::restoreLengthDistributions() {
  // create class instance using the default public constructor
  LengthDistributions my_length_distributions;

  // restore the classs
  {
    // create and open an archive for input
    std::ifstream ifs("LengthDistributions");
    ba::text_iarchive ia(ifs);
    // read class state from archive
    ia >> my_length_distributions;
  }
  return my_length_distributions;
}

// serializes flat diagonals
void Serializers::serializeFlatDiagonals(const Aligner &my_aligner_for_diagonal,
                                         const string &path_to_serialized,
                                         const string &filename) {
  // make path to seralized distributions
  const string filename_complete = path_to_serialized + filename;

  // create and open a character archive for output
  std::ofstream ofs1(filename_complete.c_str());

  // save data to archive
  {
    ba::text_oarchive oa(ofs1);
    // write class instance to archive
    oa << my_aligner_for_diagonal;
  }
}

// restores flat diagonals
Aligner Serializers::restoreFlatDiagonals(const string &path_to_serialized,
                                          const string &filename) {
  // make path to seralized distributions
  string path = path_to_serialized;
  string filename_complete = path.append(filename);

  // create class instance using the default public constructor
  Aligner my_aligner_for_diagonal = Aligner();

  // restore the classs
  {
    // create and open an archive for input
    std::ifstream ifs(filename_complete.c_str());
    ba::text_iarchive ia(ifs);
    // read class state from archive
    ia >> my_aligner_for_diagonal;
  }

  return my_aligner_for_diagonal;
}
