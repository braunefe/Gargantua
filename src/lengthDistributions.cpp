#include"LengthDistributions.h"

LengthDistributions::LengthDistributions(float param_of_poisson, map<int, float> rfs, map<int, float> rft)
{
	setParamOfPoisson(param_of_poisson);
	setRelativeFrequenciesOfSource(rfs);	
	setRelativeFrequenciesOfTarget(rft);
}

float LengthDistributions::getParamOfPoisson() const
{return param_of_poisson;}

void LengthDistributions::setParamOfPoisson(float p)
{param_of_poisson = p;}

map<int, float> LengthDistributions::getRelativeFrequenciesOfSource()
{return relative_frequencies_of_source;}

void LengthDistributions::setRelativeFrequenciesOfSource(map<int, float> s)
{relative_frequencies_of_source = s;}

map<int, float> LengthDistributions::getRelativeFrequenciesOfTarget()
{return relative_frequencies_of_target;}

void LengthDistributions::setRelativeFrequenciesOfTarget(map<int, float> t)
{relative_frequencies_of_target = t;}

//Estimates mean of poisson distribution for sentence lengths
void LengthDistributions::estimateParamOfPoisson(string input_path_source, string input_path_target)
{	

	//store corpus length for mean estimation
	float source_document_length = 0.0;
	float target_document_length = 0.0;

	float number_of_source_sentences = 0.0;
	float number_of_target_sentences = 0.0;
		
	//store paragraph lengths for variance estimation
	vector<float> source_sentence_lengths; 
	vector<float> target_sentence_lengths;
		
	vector<float> :: iterator itr_source_sent_lengths;
	vector<float> :: iterator itr_target_sent_lengths;
		
	//store square of paragraph lengths difference
	vector<float> squared_differences;
	vector<float> source_lengths; // source lengths without outliers
	
	vector<float> :: iterator itr_squared_diff;
	vector<float> :: iterator itr_source_lengths;

	//1. Train parameters
	//get names of files in source and target input directories
	stack< string > filenames;

	filenames = Utilities::findAllFiles(input_path_source);//source and target have the same name : search only in 1 directory to find filenames

	while(!filenames.empty())
	{
		string file_name = filenames.top();
		filenames.pop();

		//read Europarl input document (source and target documents)
		Data my_data = Reader::readNumberedEuroparlDocument( Utilities::findEuroparlFile(input_path_source, file_name).c_str(), Utilities::findEuroparlFile(input_path_target, file_name ).c_str());

		//get source and target sentences
		vector< Sentence > source_sentences = my_data.getSourceSentences();
		vector< Sentence > target_sentences = my_data.getTargetSentences();
		vector<Sentence> :: iterator itr_sentences;
	
		//get source and target document lengths
		//get number of source sentences
		number_of_source_sentences += (int) source_sentences.size();
		//loop through source sentences and sum sentence lengths
		for(itr_sentences=source_sentences.begin();itr_sentences!=source_sentences.end();itr_sentences++)
		{	
			Sentence sent = *itr_sentences;
			int length = sent.getWordLength();
			source_document_length += (float) length;					
		}

		//get number of  sentences 
		//loop through source sentences and sum sentence lengths
		number_of_target_sentences += (int) target_sentences.size();
		//loop through source sentences and sum sentence lengths	
		for(itr_sentences=target_sentences.begin();itr_sentences!=target_sentences.end();itr_sentences++)
		{	
			Sentence sent = *itr_sentences;
			int length = sent.getWordLength();		
			target_document_length += (float) length;
		}
	}
			
	//for debugging
	//cout << "Number of sentences in source document " << number_of_source_sentences << endl;
	//cout << "Number of sentences in target document " << number_of_target_sentences << endl;
	//cout << "Length of source document " << source_document_length << endl;
	//cout << "Length of target document " << target_document_length << "\n\n";
	
	float mean_for_source = source_document_length/number_of_source_sentences;
	//for debugging
	//fout << "Mean for source " << mean_for_source << endl;
	float mean_for_target = target_document_length/number_of_target_sentences;
	//for debugging
	//fout << "Mean for target " << mean_for_target << endl;
	float ratio = mean_for_target/mean_for_source;
	//for debugging
	//cout << "Document length ratio : " << ratio << endl;

//set poisson parameter to estimated value
setParamOfPoisson(ratio);	
}

//Computes the raw relative frequencies of sentence lengths
void LengthDistributions::computeRawRelativeFrequencies(string input_path_source, string input_path_target){

	//store corpus length for raw relative frequencies
	float source_document_length = 0.0;
	float target_document_length = 0.0;

	//map containing raw relative frequencies
	map<int, float> rel_freq_of_source;
	map<int, long int> freq_of_source;

	map<int, float> rel_freq_of_target;
	map<int, long int> freq_of_target;
		
	map<int, long int> :: iterator itr_source_freq_map;
	map<int, long int> :: iterator itr_target_freq_map;		

	//loop through the corpus
	//get names of files in source and target input directories
	stack< string > filenames;

	filenames = Utilities::findAllFiles(input_path_source);//source and target have the same name : search only in 1 directory to find filenames

	while(!filenames.empty())
	{
		string file_name = filenames.top();
		filenames.pop();

		//read Europarl input document (source and target documents)
		Data my_data = Reader::readNumberedEuroparlDocument( Utilities::findEuroparlFile(input_path_source, file_name).c_str(), Utilities::findEuroparlFile(input_path_target, file_name ).c_str());

		//get source and target sentences 
		vector<Sentence> source_sentences = my_data.getSourceSentences();
		vector<Sentence> target_sentences = my_data.getTargetSentences();	

		vector<Sentence> :: iterator itr_sent;

		//initialize frequencies (number of occurrences)
		long int frequency_source = 0;
	
			//loop through source sentences
			for(itr_sent=source_sentences.begin();itr_sent != source_sentences.end(); itr_sent++)
			{ 	
				Sentence sent = *itr_sent;
					
				//get sentence length in words
				int length = sent.getWordLength();

				//get source document length
				source_document_length += (float) length;

				//initialize frequency maps
				pair<int,int> init(length,0);
				rel_freq_of_source.insert(init);
				freq_of_source.insert(init);

				//compute frequency
				frequency_source = freq_of_source[length];
				frequency_source++;

				//replace slot
				freq_of_source.erase(length);
				pair<int, long int> fr(length,frequency_source);
				freq_of_source.insert(fr);				
			}
		
			//loop through frequency map and calculate relative frequencies
			for(itr_source_freq_map = freq_of_source.begin(); itr_source_freq_map != freq_of_source.end(); itr_source_freq_map++)
			{
				pair<int, long int> c = *itr_source_freq_map;
				
				//get frequency of curent token
				long int f= c.second;
				
				//divide by size of source document (relative frequency)
				float rf = f/source_document_length;
				
				//add to relative frequency of source (rel_freq_of_source) map
				rel_freq_of_source.erase(c.first);
				pair<int, float> rfp(c.first,rf);
				rel_freq_of_source.insert(rfp);
					
			}//end of for

			int frequency_target = 0;

			//loop through target sentences
			for(itr_sent=target_sentences.begin(); itr_sent!= target_sentences.end(); itr_sent++)
			{ 	
				Sentence sent = *itr_sent;
					
				//get sentence length in words
				int length = sent.getWordLength();

				//get target document length
				target_document_length += (float) length;

				//initialize frequency maps
				pair<int,int> init(length,0);
				rel_freq_of_target.insert(init);
				freq_of_target.insert(init);

				//compute frequency
				frequency_target = freq_of_target[length];
				frequency_target++;

				//replace cell
				freq_of_target.erase(length);
				pair<int, long int> fr(length,frequency_target);
				freq_of_target.insert(fr);	
			}//end of for
		
			//loop through frequency of target map and calculate relative frequencies
			for(itr_target_freq_map = freq_of_target.begin(); itr_target_freq_map != freq_of_target.end(); itr_target_freq_map++)
			{
				pair<int, long int> c = *itr_target_freq_map;
			
				//get frequency of target
				long int f= c.second;
				
				//divide by size of target document (relative frequency)
				float rf = f/target_document_length;
				
				//add to relative frequencyS map
				rel_freq_of_target.erase(c.first);
				pair<int, float> rfp(c.first,rf);
				rel_freq_of_target.insert(rfp);			
			}//end of for
		}//end of while

setRelativeFrequenciesOfSource(rel_freq_of_source);
setRelativeFrequenciesOfTarget(rel_freq_of_target);
}

//computes poisson log probability for source and target sentence lenghts l1 and l2
float LengthDistributions::distanceMeasurePoiss(int l1, int l2) const
{
	//get estimated parameter (ratio of source and target sentences)
	const float param = getParamOfPoisson();

	//compute the mean of poisson : ls * r (with r= mean of lengths of sentences in source/mean of length of sentences in target) 
	const float mean = param * (double) l1;
	
	//make poisson distribution
	boost::math::poisson poisson_distribution(mean);
	
	//compute probability mass function
	const double match = pdf(poisson_distribution, (double) l2);
	
	return -log(match);	
}


//write computed relative frequencies : for debugging
/*
void LengthDistributions::writeRelativeFrequencies()
{
	//optional : write raw relative frequencies of sentence lengths
	ofstream fout("length_alignment_log_files/raw_relative_frequencies.txt");
	
	map<int, float> rfs = getRelativeFrequenciesOfSource();
	map<int, float> rft = getRelativeFrequenciesOfTarget();

	map<int, float> :: iterator itrFS;	
	map<int, float> :: iterator itrFT;

	fout << "\nRELATIVE FREQUENCIES OF SOURCE" << endl;
	fout << "SIZE OF SOURCE : " << rfs.size() << endl;
	for(itrFS = rfs.begin(); itrFS != rfs.end(); itrFS++)
	{
		pair<int, float> f = *itrFS;
		fout << f.first << " : " << f.second << endl;	

	}
	
	fout << "\nRELATIVE FREQUENCIES OF TARGET" << endl;
	fout << "SIZE OF TARGET : " << rft.size() << endl;
	for(itrFT = rft.begin(); itrFT != rft.end(); itrFT++)
	{
		pair<int, float> f = *itrFT;
		fout << f.first << " : " << f.second << endl;
	}
}*/



