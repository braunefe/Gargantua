#include "Utilities.h"

namespace bfs = boost::filesystem;
namespace ba = boost::archive;
using namespace std;

// finds a file in a directory
string Utilities::findFile(const bfs::path& dir_path, string file_name) {
  // check if path is a directory
  if (!exists(dir_path) || !is_directory(dir_path)) {
    cout << "Directory : " << dir_path
         << "not found . Please enter correct directory." << endl;
    abort();
  } else {
    // iterate over the directory
    bfs::directory_iterator iter(dir_path), end_iter;
    for (; iter != end_iter; ++iter) {
      if (bfs::is_directory(*iter)) {
        // search recursively into directory
        findFile(*iter, file_name);
      }
      //       			else if( iter->leaf() == file_name )
      else if (iter->path().filename() == file_name) {
        return file_name;
      }
    }
  }

  cout << "File : " << file_name << " not found. " << endl;
  abort();
}

// converts integer to string
string Utilities::itos(int i)  // convert int to string
{
  stringstream s;
  s << i;
  return s.str();
}

// converts integer to string
float Utilities::stof(string s)  // convert string to float
{
  istringstream is(s);
  float my_float;
  is >> my_float;
  return my_float;
}

// finds all files in a directory and put filenames into vector
stack<string> Utilities::findAllFiles(const bfs::path& p) {
  // vector to hold filenames
  stack<string> filenames;
  string file_name;

  std::cout << "looking for files in " << p << std::endl;

  if (!bfs::exists(p)) {
    std::cout << "\nDirectory not found:" << p << "\n";
  }

  // if path is not a directory
  else if (!bfs::is_directory(p)) {
    //		filename_complete = p.native_file_string();
    string filename_complete = p.string();

    // get filename without .txt extensions
    size_t pos = filename_complete.rfind(".");  // position of last . in string
    if (pos != string::npos) {
      file_name = filename_complete.substr(0, pos);  // get from beginning to .
      // skip filenames starting with .
      if (file_name.size() > 0 && file_name.find(".") != 0) {
          filenames.push(file_name);
      }
    }
  }

  // else : iterate over directories
  bfs::directory_iterator iter(p), end_iter;
  for (; iter != end_iter; ++iter) {
    if (bfs::is_directory(*iter)) {
      // search recursively into directory
      findAllFiles(*iter);
    } else {
      //			string filename_complete = iter->leaf();
      const string filename_complete = iter->path().filename().string();

      // get filename without txt extension
      size_t pos = filename_complete.rfind(".");
      if (pos != string::npos) {
        // position of first . in string
        file_name =
            filename_complete.substr(0, pos);  // get from beginning to .
      } else {
        file_name = filename_complete;
      }
      // skip filenames starting with .
      if (file_name.size() > 0 && file_name.find(".") != 0) {
          filenames.push(file_name);
      }
    }
  }
  return filenames;
}

// removes a directory
void Utilities::removeDirectory(const bfs::path& p) {
  bfs::directory_iterator end;
  for (bfs::directory_iterator iter(p); iter != end; ++iter) {
    if (bfs::is_directory(*iter)) {
      removeDirectory(*iter);
    } else {
      remove(*iter);
    }
  }
  remove(p);
}

// extracts paragraph number of file to be aligned
pair<int, string> Utilities::extractParagraphNumber(string paragraph_name) {
  // regex to extract paragraph extension
  boost::regex parext_tag("(_paragraph_)(\\d*)");

  // regex to extract paragraph number
  boost::regex parnumber_tag("\\d+");

  // extract paragraph extension
  boost::sregex_token_iterator i(paragraph_name.begin(), paragraph_name.end(),
                                 parext_tag, 0);
  boost::sregex_token_iterator iend;

  string par_number_and_extension = *i++;

  // extract paragrph tag
  boost::sregex_token_iterator j(par_number_and_extension.begin(),
                                 par_number_and_extension.end(), parnumber_tag,
                                 0);
  boost::sregex_token_iterator jend;

  string par_number = *j++;

  int par_number_int = atoi(par_number.c_str());

  pair<int, string> number_and_name(par_number_int, paragraph_name);

  return number_and_name;
}

// finds files with .txt expansion
string Utilities::findEuroparlFile(const string path, const string file_name) {
  bfs::path europarl_name = path;
  europarl_name /= file_name + string(".txt");
  if (!checkFileExists(europarl_name.string())) {
    cout << "file not found: " << europarl_name << endl;
    abort();
  }
  return europarl_name.string();
}

bool Utilities::checkFileExists(const string& dir_path,
                                const string& filename) {
  bfs::path full_filename = dir_path;
  full_filename /= filename;
  ifstream f(full_filename.c_str());
  return checkFileExists(filename);
}

bool Utilities::checkFileExists(const string& filename) {
  ifstream f(filename.c_str());
  if (f.good()) {
    f.close();
    return true;
  } else {
    f.close();
    return false;
  }
}
