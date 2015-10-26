#ifndef UTILITIES_H_
#define UTILITIES_H_

#include<iostream>
#include<fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath> 
#include <set>
#include <map>
#include "boost/regex.hpp"
#include "boost/unordered_map.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/filesystem.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include <sstream>

#include "Types.h"
#include"Reader.h"
#include"Data.h"
#include"Sentence.h"
#include"LengthDistributions.h"
#include <sys/resource.h>


namespace bfs=boost::filesystem;


class Utilities {
	
	private:
	//finds file in a directory
	static string findFile(const bfs::path & dir_path, string filename);
	static bool checkFileExists(const string& filename);
	static bool checkFileExists(const string& dir_path, const string& filename);
	
	public:
	
	//converts int to string
	static string itos(int i);

	//converts string to float
	static float stof(string s);

	//finds all files in a directory and put filenames into vector
	static stack< string > findAllFiles(const bfs::path & p);

	//removes a directory
	static void removeDirectory(const bfs::path & p);

	//extracts paragraph number of file to be aligned
	static pair<int, string> extractParagraphNumber(string paragraph_name);

	//finds files with .txt expansion
	static string findEuroparlFile(string path, string file_name);	

	// converts stack to vector
	template <class T>
	static vector<T> stackToVector(stack<T>& s);

};

template <class T>
vector<T> Utilities::stackToVector(stack<T>& s) {
  vector<T> res;
  while (!s.empty()) {
    res.push_back(s.top());
    s.pop();
  }
  return res;
}

#endif /*UTILITIES_H_*/
