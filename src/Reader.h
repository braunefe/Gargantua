#ifndef READER_H_
#define READER_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath> 
#include "boost/regex.hpp"
#include "boost/algorithm/string/regex.hpp"
#include <sstream>
#include <set>

#include"Utilities.h"
#include"Sentence.h"
#include"Data.h"
#include"Writer.h"

using namespace std;
//using namespace std::tr1;
namespace bfs = boost::filesystem;


/*
Static class for reading operations
*/

class Reader {
	
	private:

	//Taggs documents i.e. taggs sentences with their unique identifiers
	static void taggDocument(const char* path_source, const char* filename_source, const char* path_target, const char* filename_target, const char* dest_source, const char* dest_target);

	//Numbers a document i.e replaces each word by an interger
	static void numberDocument(const char* path_to_source, const char* filename_source, const char* path_to_target, const char* filename_target, const char* dest_source, const char* dest_target, map<string, long int> &source_token_numbering, map<string, long int> &target_token_numbering, long int &source_counter, long int &target_counter);
	
	public:

	/*Methods to handle input documents*/

	//Taggs all Europarl documents
	static void makeIdTaggs(string path_source, string path_target, string dest_source, string dest_target);

	//Numbers all Europarl documents
	static void numberData(string path_source, string path_target, string dest_source, string dest_target);

	//Splits an Europarl document into paragraphs
	static stack<Data> splitEuroparlDocumentIntoParagraphs(const char * filename1, const char * filename2);

	//Splits Europarl document into speakers
	static stack<Data> splitEuroparlDocumentIntoSpeakers(const char * filename1, const char * filename2);

	//Splits Europarl document into chapters
	static stack<Data> splitEuroparlDocumentIntoChapters(const char * filename1, const char * filename2);

	//splits all Europarl documents into paragraphs
	static void splitEuroparlDataIntoParagraphs( string source_path, string target_path );
	
	//reads Eruoparl documents and stores information in Data object
	static Data readEuroparlDocument( const char * filename_1, const char * filename_2 );

	// reads numbered Europarl document and stores information in Data object
	static Data readNumberedEuroparlDocument(const char* filename_source, const char* filename_target);

	//reads Eruopal document wihout paragraph boundaries (already split into paragraphs) and store into vector of sentences
	static vector<Sentence> readData(const string& paragraph);

	//Looks for possibe matchings in paragraph tags 
	static int sameEuroparlParagraphs(const char * path1, const char * path2);
	
	//Looks for possibe matchings in speaker tags 
	static bool sameEuroparlSpeakers(const char * path1, const char * path2);

	//Looks for possibe matchings in chapter tags 
	static bool sameEuroparlChapters(const char * path1, const char * path2);

	/*
	Methods to handle training data
	*/

	//prepare training data from best sentence length based alignments
	static void makeTrainingData(string &path_to_best_alignments, string &path_to_training_data);

	//removes words with occurrences fewer than X from training data and replaces with the "other" word
	static pair<vector<long int>, vector<long int> > filterOutRareWordsFromTrainingData(int &occurrence_limit, const string &path_to_training_data, const string &path_to_pruned_training_data);

	//remove given lists of words from the data
	static void filterOutRareWordsFromData(vector<long int> &rare_source_words, vector<long int> &rare_target_words, string &input_path_source, string &input_path_target, string &path_to_filtered_source, string &path_to_filtered_target);

	//replaces words in training data with integers
	static void numberTrainingData(string &path_to_training_data, string &path_to_numbered_training_data);

	//makes the map indicating co-occurrence between source and target token
	static void makeCoOccurrenceMap(string &path_to_training_data, string &path_to_co_occurrence_map);

};

#endif /*READER_H_*/
