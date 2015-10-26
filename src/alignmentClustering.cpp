#include <vector>
#include <list>
#include <iostream> 
#include <cmath>
#include "Chunk.h" 
#include "AlignmentClustering.h"
#include "LengthDistributions.h"
#include "LexicalDistributions.h"
#include "Cell.h"
#include "boost/shared_ptr.hpp" 

typedef boost::shared_ptr<Cell> CellPtr;

AlignmentClustering::AlignmentClustering(stack< Chunk > sentence_alignment)
{
	setSentenceAlignment(sentence_alignment);
}

stack<Chunk> AlignmentClustering::getSentenceAlignment()
{
	return  sentence_alignment;
}

void AlignmentClustering::setSentenceAlignment(stack<Chunk> sent_align)
{
	sentence_alignment = sent_align;
}

stack<Chunk> AlignmentClustering::getExpandedAlignment()
{
	return  expanded_alignment;
}

void AlignmentClustering::setExpandedAlignment(stack<Chunk> exp_align)
{
	expanded_alignment = exp_align;
}

//compute bead priors (P_ak()) and lambda factor for successions of insertions and deletions 
	//go through initial alignment and count possible candidates
void AlignmentClustering :: estimatePriors(stack<Chunk> a)
{
	//candidates
	float candidates_10 = 0.0f;
	float candidates_11 = 0.0f;
	float candidates_21 = 0.0f;
	float candidates_31 = 0.0f;
	float candidates_41 = 0.0f;
	float total_candidates = 0.0f;

	//bloc sizes
	float bloc_counter = 0.0f;
	float bloc_size = 0.0f;
	float total_bloc_size = 0.0f;

	//priors
	float prior11 = 0.0f;
	float prior10 = 0.0f;	
	float prior12 = 0.0f;
	float prior13 = 0.0f;
	float prior14 = 0.0f;
	float weight = 0.0f;

	//put chunks into vector
	stack<Chunk> reverse_alignment;	
	stack<Chunk> alignment_temp = a;
	stack<Chunk> alignment_temp_2 = a;
	
	while(!alignment_temp_2.empty())
	{	
		Chunk current = alignment_temp_2.top();
		alignment_temp_2.pop();
	}

	//number each alignment
	int pass_counter = 0;

	//store size of previous alignments (until 3 before current (for 1-to-4/4-to-1 clusters)
	int previous_source_size = 0;
	int previous_target_size = 0;
	int source_size_two_before = 0;
	int target_size_two_before = 0;
	int source_size_three_before = 0;
	int target_size_three_before = 0;
	int nbr_blocs = 0;
	
	//to store source and target chunks
	vector< vector<long int> > current_source_chunk_token;
	vector< vector<long int> > current_target_chunk_token;

	map<int, float> distances;

	while(!alignment_temp.empty())
	{
		pass_counter++;
		
		Chunk current_chunk = alignment_temp.top();
		alignment_temp.pop();

		//get source and target chunk
		current_source_chunk_token = current_chunk.getSourceChunkToken();
		current_target_chunk_token = current_chunk.getTargetChunkToken();		

		int current_source_size = current_source_chunk_token.size();
		int current_target_size = current_target_chunk_token.size();
		
		//merge sizes
		int merge21_source_size = 0;
		int merge21_target_size = 0;
		int merge31_source_size = 0;
		int merge31_target_size = 0;
		int merge41_source_size = 0;
		int merge41_target_size = 0;
	
		//avoid multiple counts of mergers
		int previous_candidate_21 = 0;
		int previous_candidate_31 = 0;
		int previous_candidate_41 = 0;

		//count mergers
		if(pass_counter > 1)
		{
			merge21_source_size = current_source_size + previous_source_size;
			merge21_target_size = current_target_size + previous_target_size;
		}
		if(pass_counter > 2)
		{
			merge31_source_size = current_source_size + previous_source_size + source_size_two_before;
			merge31_target_size = current_target_size + previous_target_size + target_size_two_before;
		}		
		if(pass_counter > 3)
		{
			merge41_source_size = current_source_size + previous_source_size + source_size_two_before + source_size_three_before;
			merge41_target_size = current_target_size + previous_target_size + target_size_two_before + target_size_three_before;
		}
		
		//count blocs
		//count blocs of more than 1 insertion or deletion
		if(merge21_source_size == 0 || merge21_target_size == 0) //merge21 = current size + previous size
		{	
			bloc_counter++;
		}
		else
		{
			bloc_size = bloc_counter;
			total_bloc_size += bloc_size;
			if(bloc_size > 3)
			{nbr_blocs++;}
			bloc_counter= 0;
		}

		//count candidates for beads of specific size

		//number of deletions		
		if(current_source_size == 0 || current_target_size == 0) 
		{	
			candidates_10++;
		}

		//number of substitutions
		if(current_source_size == 1 && current_target_size == 1)
		{
			candidates_11++;
		}
		
		//merger 2-1
		if((merge21_source_size == 1 && merge21_target_size > 1) ||(merge21_target_size == 1 && merge21_source_size > 1))
		{
			if(previous_candidate_21 == 0)
			{candidates_21++;}
			previous_candidate_21++;

		}
		else{previous_candidate_21 = 0;}

		//merger 3-1
		if((merge31_source_size == 1 && merge31_target_size > 2) ||(merge31_target_size == 1 && merge31_source_size > 2))
		{
			if(previous_candidate_31 == 0)
			{candidates_31++;}
			previous_candidate_31++;
		}
		else{previous_candidate_41 = 0;}

		//merger 4-1
		if((merge41_source_size == 1 && merge41_target_size > 3) ||(merge41_target_size == 1 && merge41_source_size > 3))
		{
			if(previous_candidate_41 == 0)
			{candidates_41++;}
			previous_candidate_41++;
		}
		else{previous_candidate_41 = 0;}
		
		//compute new sizes
		target_size_three_before = target_size_two_before;
		source_size_three_before = source_size_two_before;
		target_size_two_before = previous_target_size;
		source_size_two_before = previous_source_size;
		previous_target_size = current_target_size;
		previous_source_size = current_source_size;

 	}//end of while	

	//for debugging
	//cout << total_bloc_size << endl;
	double ratio = (candidates_21 + candidates_31 + candidates_41) / (double) total_bloc_size;

	//put weigths depending on ration between candidates for mergers and total bloc size
	if(ratio > 5){weight = 1.0;}
	if(ratio <= 5)
	{weight = 0.01;}
	if(ratio < 2.5)
	{weight = 0.001;}
	if(ratio < 1.25)
	{weight = 0.0001;}
	
	//for debugging
	//cout << "weight " << weight << endl;

	//sum over all possible candidates
	total_candidates = candidates_21 + candidates_31 + candidates_41 + candidates_10 + candidates_11;
	prior12 = weight * (candidates_21 /total_candidates);
	prior13 = weight * (candidates_31 /total_candidates);
	prior14 = weight * (candidates_41 /total_candidates);
	
	//prior11 and prior10 = "empty" prior i.e. prior probability that no alignment occurrs
	prior11 = 1.0f - (prior12 + prior13 + prior14); 
	prior10 = 1.0f - (prior12 + prior13 + prior14); 

	//set priors at global scope
	prior_11 = prior11;
	prior10_01 = prior10;		
	prior12_21 = prior12;
	prior13_31 = prior13;
	prior14_41 = prior14;
}

//distance function for making merging decisions (based on distance function as defined in Moore 2002 but with Viterbi model 1 instead of summing up over all alignments)
long double AlignmentClustering :: mergeDistanceFunction(LengthDistributions &length_distributions, LexicalDistributions &lexical_distributions, vector<long int> st, vector<long int> tt, vector<int> ss, vector<int> ts)
{
	int s1 = 0;
	if(ss.empty()){s1 = 0;}
	else{s1 = ss.front();}
	
	int t1 = 0;
	if(ts.empty()){t1 = 0;}
	else{t1 = ts.front();}

	if(ss.size() == 0) //insertion 
	{	
		//compute length statistics	
		float relft = 0.0f;
		if(length_distributions.relative_frequencies_of_source.find(t1) != length_distributions.relative_frequencies_of_source.end())
		{relft = length_distributions.relative_frequencies_of_source.find(t1)->second;} 
		//if raw relative frequency not found put to uniform
		else{relft = 1.0/length_distributions.relative_frequencies_of_target.size();}
		
		float lengthDist = -log(relft);

		vector<long int> :: iterator itr_target_token;
			
		//compute unigram frequencies
		float uf = 0.0f;
		float wordDist = 0.0f;
						
		for(itr_target_token = tt.begin(); itr_target_token != tt.end(); itr_target_token++)
		{
			long int token = *itr_target_token;
				
			if(lexical_distributions.unigram_frequencies_of_target.find(token) != lexical_distributions.unigram_frequencies_of_target.end())
			{
				uf = lexical_distributions.unigram_frequencies_of_target.find(token)->second;
			}
			else
			{uf =  0.000001f;}//very low probability for words that have not been found
			wordDist += -log(uf);
		}
		
		float distanceI = lengthDist + wordDist -log(prior10_01);			
		return distanceI;
			
	}
	
	else if(ts.size() == 0) //deletion
	{
		//compute length statistics
		float relfs = 0.0f;
		if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
		{relfs = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
		//if raw relative frequency not found put to uniform
		else{relfs = 1.0/length_distributions.relative_frequencies_of_source.size();}			
		
		float lengthDist = -log(relfs);
		
		vector<long int> :: iterator itr_source_token;
			
		//compute unigram frequencies
		float uf = 0.0f;
		float wordDist = 0.0f;
						
		for(itr_source_token = st.begin(); itr_source_token != st.end(); itr_source_token++)
		{
			float token = *itr_source_token;
		
			if(lexical_distributions.unigram_frequencies_of_source.find(token) != lexical_distributions.unigram_frequencies_of_source.end())
			{
				uf = lexical_distributions.unigram_frequencies_of_source.find(token)->second;
			}
			else
			{uf =  0.000001f;}//very low probability for words that have not been found
			wordDist += -log(uf);
		}

		float distanceD = lengthDist + wordDist -log(prior10_01);	
		return distanceD;
	}
	else{// all other cases 

		float lengthDist = 0.0f;

		//compute length statistics for substitution
		if(ss.size() == 1 && ts.size() == 1)
		{
			float distance = length_distributions.distanceMeasurePoiss(s1,t1);
			float relfs = 0.0f;
			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			//if raw relative frequency not found put to uniform
			else{relfs = 1.0/length_distributions.relative_frequencies_of_source.size();}

			lengthDist = distance - log(relfs) -log(prior_11);
		}
			
		//compute length statistics for merger 1-to-2
		else if(ss.size() == 1 && ts.size() == 2)
		{
			int t2 = ts.at(1);

			float distance = length_distributions.distanceMeasurePoiss(s1, t1 + t2);
			float relfs = 0.0f;
			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			//if not found put to uniform
			else{relfs = 1.0/length_distributions.relative_frequencies_of_source.size();}
			lengthDist = distance - log(relfs) -log(prior12_21);
		}

		//compute length statistics for merger 1-to-3
		else if(ss.size() == 1 && ts.size() == 3)
		{
			int t2 = ts.at(1);
			int t3 = ts.at(2);

			float distance = length_distributions.distanceMeasurePoiss(s1, t1 + t2 + t3);
			float relfs = 0.0f;
			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			//if not found put to uniform		
			else{relfs = 1.0/length_distributions.relative_frequencies_of_source.size();}
			lengthDist = distance - log(relfs) -log(prior13_31);
		}

		//compute length statistics for merger 1-to-4
		else if(ss.size() == 1 && ts.size() == 4)
		{
			int t2 = ts.at(1);
			int t3 = ts.at(2);
			int t4 = ts.at(3);

			float distance = length_distributions.distanceMeasurePoiss(s1, t1 + t2 + t3 + t4);
			float relfs = 0.0f;
			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			//if not found put to uniform		
			else{relfs = 1.0/length_distributions.relative_frequencies_of_source.size();}
			lengthDist = distance - log(relfs) -log(prior14_41);
		}
		
		//compute length statistics for merger 1-to-5
		else if(ss.size() == 2 && ts.size() == 1)
		{
			int s2 = ss.at(1);

			float distance = length_distributions.distanceMeasurePoiss(s1 + s2, t1);
			
			float relfs1 = 0.0f;
			float relfs2 = 0.0f;
			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs1 = length_distributions.relative_frequencies_of_source.find(s1)->second;}
			//if not found put to uniform 
			else{
			relfs1 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			if(length_distributions.relative_frequencies_of_source.find(s2) != length_distributions.relative_frequencies_of_source.end())
			{relfs2 = length_distributions.relative_frequencies_of_source.find(s2)->second;} 
			//if not found put to uniform 
			else{
			relfs2 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			lengthDist = distance - log(relfs1) - log(relfs2) -log(prior12_21);
		}
		//compute length statistics for merger 3-to-1
		else if(ss.size() == 3 && ts.size() == 1)
		{
			int s2 = ss.at(1);
			int s3 = ss.at(2);

			float distance = length_distributions.distanceMeasurePoiss(s1 + s2 + s3, t1);
			
			float relfs1 = 0.0f;
			float relfs2 = 0.0f;
			float relfs3 = 0.0f;

			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs1 = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			else{
			//if not found put to uniform 
			relfs1 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			if(length_distributions.relative_frequencies_of_source.find(s2) != length_distributions.relative_frequencies_of_source.end())
			{relfs2 = length_distributions.relative_frequencies_of_source.find(s2)->second;} 
			else{
			//if not found put to uniform 
			relfs2 = 1.0/(length_distributions.relative_frequencies_of_source.size());}
			if(length_distributions.relative_frequencies_of_source.find(s3) != length_distributions.relative_frequencies_of_source.end())
			{relfs3 = length_distributions.relative_frequencies_of_source.find(s3)->second;} 
			else{
			//if not found put to uniform 
			relfs3 = 1.0/(length_distributions.relative_frequencies_of_source.size());}
			lengthDist = distance -log(relfs1) -log(relfs2) -log(relfs3) -log(prior13_31);
		}
		//compute length statistics for merger 4 to 1
		else if(ss.size() == 4 && ts.size() == 1)
		{
			int s2 = ss.at(1);
			int s3 = ss.at(2);
			int s4 = ss.at(3);

			long double distance = length_distributions.distanceMeasurePoiss(s1 + s2 + s3 + s4, t1);
			
			long double relfs1 = 0.0f;
			long double relfs2 = 0.0f;
			long double relfs3 = 0.0f;
			long double relfs4 = 0.0f;

			if(length_distributions.relative_frequencies_of_source.find(s1) != length_distributions.relative_frequencies_of_source.end())
			{relfs1 = length_distributions.relative_frequencies_of_source.find(s1)->second;} 
			else{
			//if not found put to uniform 
			relfs1 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			if(length_distributions.relative_frequencies_of_source.find(s2) != length_distributions.relative_frequencies_of_source.end())
			{relfs2 = length_distributions.relative_frequencies_of_source.find(s2)->second;} 
			else{
			//if not found put to uniform 
			relfs2 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			if(length_distributions.relative_frequencies_of_source.find(s3) != length_distributions.relative_frequencies_of_source.end())
			{relfs3 = length_distributions.relative_frequencies_of_source.find(s3)->second;} 
			else{
			//if not found put to uniform 
			relfs3 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			if(length_distributions.relative_frequencies_of_source.find(s4) != length_distributions.relative_frequencies_of_source.end())
			{relfs4 = length_distributions.relative_frequencies_of_source.find(s4)->second;} 
			else{
			//if not found put to uniform 
			relfs4 = 1.0/length_distributions.relative_frequencies_of_source.size();}
			lengthDist = distance -log(relfs1) -log(relfs2) -log(relfs3) -log(relfs4) -log(prior14_41);
		}
		
		vector<long int> :: iterator itr_source_token;
		vector<long int> :: iterator itr_target_token;

		//compute unigram frequencies
		float uf = 0.0f;
		float wordDist = 0.0f;
						
		for(itr_source_token = st.begin(); itr_source_token != st.end(); itr_source_token++)
		{
			long int token = *itr_source_token;
			if(lexical_distributions.unigram_frequencies_of_source.find(token) != lexical_distributions.unigram_frequencies_of_source.end())
			{uf = lexical_distributions.unigram_frequencies_of_source.find(token)->second;}
			wordDist += -log(uf);
		}

		//put null word at the end of the source token
		st.push_back(0);
		
		//trans proba of target given source
		long double transProb = 0.0f;

		//total translation probabilites for source words
		double totalTPST = 0.0f;
			
		//maximum translation probability for source words
		double maxTP = -log(0.0f);

		//compute model 1 score
			//loop through token of target sentence
		for(itr_target_token = tt.begin(); itr_target_token != tt.end(); itr_target_token++)
		{
			long int ttok = *itr_target_token;

			maxTP = -log(0.0f);
			//loop through token of source sentence
			for(itr_source_token = st.begin(); itr_source_token != st.end(); itr_source_token++)
			{	
				long int stok = *itr_source_token;
				//check if words are in translation table
				//If yes, get proba;
				if(lexical_distributions.translation_table.find(stok) != lexical_distributions.translation_table.end())
				{	
					if(lexical_distributions.translation_table.find(stok)->second.find(ttok) != lexical_distributions.translation_table.find(stok)->second.end())
					{
						transProb = -log(lexical_distributions.translation_table.find(stok)->second.find(ttok)->second);
						if(transProb <= maxTP)
						{
							maxTP = transProb;
						}
					}
					//else proba is taken from null
					else 
					{	
						if(lexical_distributions.translation_table.find(0)->second.find(ttok) != lexical_distributions.translation_table.find(0)->second.end())
						{
							transProb = -log(lexical_distributions.translation_table.find(0)->second.find(ttok)->second);
							if(transProb <= maxTP)
							{
								maxTP = transProb;	
							}
						}
						else{
							transProb = -log(0.0000001f);//very low probability for words that have not been found
							if(transProb <= maxTP)
							{
								maxTP = transProb;	
							}
						}
					}		
				}//end of if
				//else take proba for the null word
				else 
				{	
					if( lexical_distributions.translation_table.find(0)->second.find(ttok) !=  lexical_distributions.translation_table.find(0)->second.end())
					{
						transProb = -log(lexical_distributions.translation_table.find(0)->second.find(ttok)->second);
						if(transProb <= maxTP)
						{
							maxTP = transProb;	
						}
					}
					else{
						transProb = -log(0.0000001f);//very low probability for words that have not been found
						if(transProb <= maxTP)
						{
							maxTP = transProb;	
						}
					}
				}
			}//end of loop through source token
			totalTPST += maxTP;
		}//end of loop through target tok
	
		//compute Viterbi model-1 probability 
		long double model_1 = totalTPST + log( ( (float) st.size() + 1.0f ) );
			
		long double end_distance = lengthDist + model_1 + wordDist;
		return end_distance; 

	}//end of else
}

//dynamic programming procedure for alignment clustering
void AlignmentClustering :: alignmentClustering(stack<Chunk> a, LengthDistributions &l_d, LexicalDistributions &lex_d)
{
	stack<Chunk> alignment_temp = a; //stack used for precomputation of distances

	//new alignment	
	stack<Chunk> expanded_alignment_temp;
	stack<Chunk> expanded_alignment;

	//infinite distance
	const long double MAXD = numeric_limits<long double>::infinity( );		

	//null vector
	vector<long int> nullV;
	
	//null chunk
	vector< vector<long int> > nullC;

	//distances of mergers
	long double distance_1 = MAXD; //current distance
	long double distance_2 = MAXD; //distance of merged 2-1
	long double distance_3 = MAXD; //distance of merged 3-1
	long double distance_4 = MAXD; //distance of merged 4-1

	//null values for cell fields
	long double null_distance = 0.0;
	pair<int,int> null_previous;
	null_previous.first = -1;
	null_previous.second = -1;

	//max value for cell fields
	long double max_distance = MAXD;

	//DistanceMatrix declaration
		//distance Matrix
	map < int, CellPtr > distanceMatrix;

		//trace back
	stack< pair<int, int> > trace_back;


//1. Precompute distances and store into a distance map

	//put chunks into vector
	vector<Chunk> first_alignment;
	stack<Chunk> reverse_alignment_1; //reverse alignment for distance computation
	stack<Chunk> reverse_alignment_2; //reverse alignment for building clustered alignment
	stack<Chunk> reverse_alignment_temp;	

	stack<Chunk> alignment_temp_2 = a;

	//store previous chunk sizes	
	int previous_source_size = 0;
	int previous_target_size = 0;
	int source_size_two_before = 0;
	int target_size_two_before = 0;
	int source_size_three_before = 0;
	int target_size_three_before = 0;
	int source_size_four_before = 0;
	int target_size_four_before = 0;
	
	//reverse alignment
	while(!alignment_temp_2.empty())
	{	
		Chunk current = alignment_temp_2.top();
		alignment_temp_2.pop();
		reverse_alignment_1.push(current);
		reverse_alignment_2.push(current);
		reverse_alignment_temp.push(current);
	}

	//reverse alignment
	while(!reverse_alignment_temp.empty())
	{	
		Chunk current = reverse_alignment_temp.top();
		first_alignment.push_back(current);
		reverse_alignment_temp.pop();
	}

	//number of each alignment
	int pass_counter = 0;

	//label in the distance map
	int label = 0;

	//store last merger distance
	long double distance_of_merged = 0;

	//to get the size of the mergers
	vector< vector<long int> > current_source_chunk_token;
	vector< vector<long int> > current_target_chunk_token;

	//map containing precomputed distances (long double) and corresponding labels (int)
	map<int, long double> distances;

	while(!reverse_alignment_1.empty())
	{
		pass_counter++;	
		
		label = pass_counter * 10;

		Chunk current_chunk = reverse_alignment_1.top();
		reverse_alignment_1.pop();

		//current source token
		current_source_chunk_token = current_chunk.getSourceChunkToken();
		current_target_chunk_token = current_chunk.getTargetChunkToken();		

		int current_source_size = current_source_chunk_token.size();
		int current_target_size = current_target_chunk_token.size();
	
		int merge21_source_size = 1;
		int merge21_target_size = 1;
		int merge31_source_size = 1;
		int merge31_target_size = 1;
		int merge41_source_size = 1;
		int merge41_target_size = 1;

		//Comppute merge sizes
		if(pass_counter > 1)
		{
			merge21_source_size = current_source_size + previous_source_size;
			merge21_target_size = current_target_size + previous_target_size;
		}
		if(pass_counter > 2)
		{
			merge31_source_size = current_source_size + previous_source_size + source_size_two_before;
			merge31_target_size = current_target_size + previous_target_size + target_size_two_before;
		}		
		if(pass_counter > 3)
		{
			merge41_source_size = current_source_size + previous_source_size + source_size_two_before + source_size_three_before;
			merge41_target_size = current_target_size + previous_target_size + target_size_two_before + target_size_three_before;
		}

		//for 1-to-many and many-to-1 relations : 	
			//compute and compare distance functions as in the dynamic programming framework		
		if((merge21_source_size == 1 && merge21_target_size > 1) ||(merge21_target_size == 1 && merge21_source_size > 1))
		{
			int first_position = pass_counter -2; //vector begins at 0
			int second_position = pass_counter -1;

			Chunk first_chunk = first_alignment.at(first_position);
			Chunk second_chunk = first_alignment.at(second_position);

			vector< vector<long int> > first_source_chunk_token = first_chunk.getSourceChunkToken();
			vector< vector<long int> > first_target_chunk_token = first_chunk.getTargetChunkToken();
			
			vector< vector<long int> > second_source_chunk_token = second_chunk.getSourceChunkToken();
			vector< vector<long int> > second_target_chunk_token = second_chunk.getTargetChunkToken();

			vector<long int> first_source_token;
			vector<long int> first_target_token;

			vector<long int> second_source_token;
			vector<long int> second_target_token;

			vector<long int> merged_source_token;
			vector<long int> merged_target_token;

			vector<int> first_source_size;
			vector<int> first_target_size;
		
			vector<int> second_source_size;
			vector<int> second_target_size;

			vector<int> merged_source_size;
			vector<int> merged_target_size;

			vector< vector<long int > > :: iterator itr_chunk_tok;

			//put merged token together
			for(itr_chunk_tok = first_source_chunk_token.begin(); itr_chunk_tok != first_source_chunk_token.end(); itr_chunk_tok++ )
			{
				vector<long int> token = *itr_chunk_tok;				
				vector<long int> :: iterator itr_tok;
				first_source_size.push_back(token.size());
				merged_source_size.push_back(token.size());
				
				for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
				{
					long int t = *itr_tok;
					merged_source_token.push_back(t);
					first_source_token.push_back(t);	
				}
			}
		
			for(itr_chunk_tok = second_source_chunk_token.begin(); itr_chunk_tok != second_source_chunk_token.end(); itr_chunk_tok++ )
			{
				vector<long int> token = *itr_chunk_tok;				
				vector<long int> :: iterator itr_tok;
				second_source_size.push_back(token.size());
				merged_source_size.push_back(token.size());
				for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
				{
					long int t = *itr_tok;
					merged_source_token.push_back(t);
					second_source_token.push_back(t);	
				}
			}	
		
			for(itr_chunk_tok = first_target_chunk_token.begin(); itr_chunk_tok != first_target_chunk_token.end(); itr_chunk_tok++ )
			{
				vector<long int> token = *itr_chunk_tok;				
				vector<long int> :: iterator itr_tok;
				merged_target_size.push_back(token.size());
				first_target_size.push_back(token.size());
				for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
				{
					long int t = *itr_tok;
					merged_target_token.push_back(t);
					first_target_token.push_back(t);	
				}
			}	

			for(itr_chunk_tok = second_target_chunk_token.begin(); itr_chunk_tok != second_target_chunk_token.end(); itr_chunk_tok++ )
			{
				vector<long int> token = *itr_chunk_tok;				
				vector<long int> :: iterator itr_tok;
				merged_target_size.push_back(token.size());
				second_target_size.push_back(token.size());
				for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
				{
					long int t = *itr_tok;
					merged_target_token.push_back(t);
					second_target_token.push_back(t);	
				}
			}				

			//compute distances
			//distance of previous
			long double distance_of_previous_chunk = mergeDistanceFunction(	l_d, lex_d, first_source_token, first_target_token, 
											first_source_size, first_target_size );
			
			//distance of current
			long double distance_of_current_chunk = mergeDistanceFunction( l_d, lex_d, second_source_token, second_target_token,
											second_source_size, second_target_size );
			
			//distance of merged	
			distance_of_merged = mergeDistanceFunction(l_d, lex_d, merged_source_token, merged_target_token, merged_source_size, merged_target_size);


			//store distance of merged chunks (2-1)
			pair<int, long double> new_distance;
			int label_merged = (pass_counter * 10 ) + 2;
			new_distance.first = label_merged; //2-1 mergers are labelled x2
			new_distance.second = distance_of_merged;

			if(distances.find(label_merged) == distances.end())
			{
				distances.insert(new_distance);
			}
			else
			{
				distances.erase(label_merged);
				distances.insert(new_distance);
				
			}

			//store distance of first
			int label_first = ((pass_counter -1) * 10 ) + 1;
			new_distance.first = label_first; //empty merged are labelled x1
			new_distance.second = distance_of_previous_chunk;
			
			if(distances.find(label_first) == distances.end())
			{
				distances.insert(new_distance);
			}
			else
			{
				distances.erase(label_first);
				distances.insert(new_distance);
				
			}

			//store distance of second
			int label_second = (pass_counter * 10 ) + 1;
			new_distance.first = label_second;
			new_distance.second = distance_of_current_chunk;

			
			if(distances.find(label_second) == distances.end())
			{
				distances.insert(new_distance);
			}
			else
			{
				distances.erase(label_second);
				distances.insert(new_distance);
			}

			first_chunk.clear();
			second_chunk.clear();

			first_source_chunk_token.clear();
			first_target_chunk_token.clear();
	
			second_source_chunk_token.clear();
			second_target_chunk_token.clear();

			first_source_token.clear();
			first_target_token.clear();

			second_source_token.clear();
			second_target_token.clear();

			merged_source_token.clear();
			merged_target_token.clear();

		}

		//merger 3-1
		if((merge31_source_size == 1 && merge31_target_size > 2) ||(merge31_target_size == 1 && merge31_source_size > 2))
		{

				int first_position = pass_counter-3; //vector begins at 0
				int second_position = pass_counter-2;
				int third_position = pass_counter-1;

				Chunk first_chunk = first_alignment.at(first_position);
				Chunk second_chunk = first_alignment.at(second_position);
				Chunk third_chunk = first_alignment.at(third_position);

				vector< vector<long int> > first_source_chunk_token = first_chunk.getSourceChunkToken();
				vector< vector<long int> > first_target_chunk_token = first_chunk.getTargetChunkToken();
			
				vector< vector<long int> > second_source_chunk_token = second_chunk.getSourceChunkToken();
				vector< vector<long int> > second_target_chunk_token = second_chunk.getTargetChunkToken();

				vector< vector<long int> > third_source_chunk_token = third_chunk.getSourceChunkToken();
				vector< vector<long int> > third_target_chunk_token = third_chunk.getTargetChunkToken();

				vector< vector<long int > > :: iterator itr_chunk_tok;

				vector<long int> first_source_token;
				vector<long int> first_target_token;

				vector<long int> second_source_token;
				vector<long int> second_target_token;

				vector<long int> third_source_token;
				vector<long int> third_target_token;

				vector<long int> merged_source_token;
				vector<long int> merged_target_token;

				vector<int> merged_source_size;
				vector<int> merged_target_size;

				//put merged token together
				for(itr_chunk_tok = first_source_chunk_token.begin(); itr_chunk_tok != first_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());				

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						first_source_token.push_back(t);	
					}
				}
		
				for(itr_chunk_tok = second_source_chunk_token.begin(); itr_chunk_tok != second_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						second_source_token.push_back(t);	
					}
				}	
			
				for(itr_chunk_tok = third_source_chunk_token.begin(); itr_chunk_tok != third_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						third_source_token.push_back(t);	
					}
				}	

		
				for(itr_chunk_tok = first_target_chunk_token.begin(); itr_chunk_tok != first_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						first_target_token.push_back(t);	
					}
				}	

				for(itr_chunk_tok = second_target_chunk_token.begin(); itr_chunk_tok != second_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						second_target_token.push_back(t);	
					}
				}

				for(itr_chunk_tok = third_target_chunk_token.begin(); itr_chunk_tok != third_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						third_target_token.push_back(t);	
					}
				}		
	
				//compute distance of merger
				distance_of_merged = mergeDistanceFunction(l_d, lex_d, merged_source_token, merged_target_token, merged_source_size, merged_target_size);

				//store distance of merged chunks (3-1)
				pair<int, long double> new_distance;
				int label_merged_31 = (pass_counter * 10 ) + 3;
				new_distance.first = label_merged_31; //3-1 mergers are labelled x3
				new_distance.second = distance_of_merged;
				

				if(distances.find(label_merged_31) == distances.end())
				{
					distances.insert(new_distance);
				}
				else
				{
					distances.erase(label_merged_31);
					distances.insert(new_distance);
				}

				first_chunk.clear();
				second_chunk.clear();
				third_chunk.clear();

				first_source_chunk_token.clear();
				first_target_chunk_token.clear();
			
				second_source_chunk_token.clear();
				second_target_chunk_token.clear();

				third_source_chunk_token.clear();
				third_target_chunk_token.clear();

				first_source_token.clear();
				first_target_token.clear();

				second_source_token.clear();
				second_target_token.clear();
				
				third_source_token.clear();
				third_target_token.clear();

				merged_source_token.clear();
				merged_target_token.clear();	

		}
		
		//merger 4-1
		if((merge41_source_size == 1 && merge41_target_size > 3) ||(merge41_target_size == 1 && merge41_source_size > 3))
		{

				int first_position = pass_counter -4;
				int second_position = pass_counter -3;
				int third_position = pass_counter -2;
				int fourth_position = pass_counter -1;

				Chunk first_chunk = first_alignment.at(first_position);
				Chunk second_chunk = first_alignment.at(second_position);
				Chunk third_chunk = first_alignment.at(third_position);
				Chunk fourth_chunk = first_alignment.at(fourth_position);

				vector< vector<long int> > first_source_chunk_token = first_chunk.getSourceChunkToken();
				vector< vector<long int> > first_target_chunk_token = first_chunk.getTargetChunkToken();
			
				vector< vector<long int> > second_source_chunk_token = second_chunk.getSourceChunkToken();
				vector< vector<long int> > second_target_chunk_token = second_chunk.getTargetChunkToken();

				vector< vector<long int> > third_source_chunk_token = third_chunk.getSourceChunkToken();
				vector< vector<long int> > third_target_chunk_token = third_chunk.getTargetChunkToken();

				vector< vector<long int> > fourth_source_chunk_token = fourth_chunk.getSourceChunkToken();
				vector< vector<long int> > fourth_target_chunk_token = fourth_chunk.getTargetChunkToken();

				vector< vector<long int > > :: iterator itr_chunk_tok;

				vector<long int> first_source_token;
				vector<long int> first_target_token;

				vector<long int> second_source_token;
				vector<long int> second_target_token;

				vector<long int> third_source_token;
				vector<long int> third_target_token;

				vector<long int> fourth_source_token;
				vector<long int> fourth_target_token;

				vector<long int> merged_source_token;
				vector<long int> merged_target_token;

				vector<int> merged_source_size;
				vector<int> merged_target_size;

				//put merged token together
				for(itr_chunk_tok = first_source_chunk_token.begin(); itr_chunk_tok != first_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						first_source_token.push_back(t);	
					}
				}
		
				for(itr_chunk_tok = second_source_chunk_token.begin(); itr_chunk_tok != second_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						second_source_token.push_back(t);	
					}
				}	
			
				for(itr_chunk_tok = third_source_chunk_token.begin(); itr_chunk_tok != third_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						third_source_token.push_back(t);	
					}
				}
			
				for(itr_chunk_tok = fourth_source_chunk_token.begin(); itr_chunk_tok != fourth_source_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_source_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_source_token.push_back(t);
						fourth_source_token.push_back(t);	
					}
				}	

		
				for(itr_chunk_tok = first_target_chunk_token.begin(); itr_chunk_tok != first_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						first_target_token.push_back(t);	
					}
				}	

				for(itr_chunk_tok = second_target_chunk_token.begin(); itr_chunk_tok != second_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						second_target_token.push_back(t);	
					}
				}

				for(itr_chunk_tok = third_target_chunk_token.begin(); itr_chunk_tok != third_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						third_target_token.push_back(t);	
					}
				}

				for(itr_chunk_tok = fourth_target_chunk_token.begin(); itr_chunk_tok != fourth_target_chunk_token.end(); itr_chunk_tok++ )
				{
					vector<long int> token = *itr_chunk_tok;				
					vector<long int> :: iterator itr_tok;
					merged_target_size.push_back(token.size());

					for(itr_tok = token.begin(); itr_tok != token.end(); itr_tok++)
					{
						long int t = *itr_tok;
						merged_target_token.push_back(t);
						fourth_target_token.push_back(t);	
					}
				}


				//compute distence of merger
				distance_of_merged = mergeDistanceFunction(l_d, lex_d, merged_source_token, merged_target_token, merged_source_size, merged_target_size);

				//store distance of merged chunks (4-1)
				pair<int, long double> new_distance;				
				int label_merged_41 = (pass_counter * 10 ) + 4;
				new_distance.first = label_merged_41; //4-1 mergers are labelled x4
				new_distance.second = distance_of_merged;

				if(distances.find(label_merged_41) == distances.end())
				{
					distances.insert(new_distance);
				}
				else
				{
					distances.erase(label_merged_41);
					distances.insert(new_distance);
				}			

				first_chunk.clear();
				second_chunk.clear();
				third_chunk.clear();
				fourth_chunk.clear();

				first_source_chunk_token.clear();
				first_target_chunk_token.clear();
			
				second_source_chunk_token.clear();
				second_target_chunk_token.clear();

				third_source_chunk_token.clear();
				third_target_chunk_token.clear();

				fourth_source_chunk_token.clear();
				fourth_target_chunk_token.clear();

				first_source_token.clear();
				first_target_token.clear();

				second_source_token.clear();
				second_target_token.clear();
				
				third_source_token.clear();
				third_target_token.clear();

				fourth_source_token.clear();
				fourth_target_token.clear();

				merged_source_token.clear();
				merged_target_token.clear();	
		
		}
		else
		{
			//if no candidate for merge, distance of current chunk labelled with 0
			pair<int, long double> new_distance;
			new_distance.first = (pass_counter * 10 ) + 1;
			new_distance.second = 0.0;
			distances.insert(new_distance);	
			
		}
	
		//compute new sizes
		target_size_four_before = target_size_three_before;
		source_size_four_before = source_size_three_before;
		target_size_three_before = target_size_two_before;
		source_size_three_before = source_size_two_before;
		target_size_two_before = previous_target_size;
		source_size_two_before = previous_source_size;
		previous_target_size = current_target_size;
		previous_source_size = current_source_size;

	}//end of while	

//2. Perform dynamic programming search

	//construct distance matrix
	for(int i = 0; i <= (int) a.size(); i++) //loop trough alignment outputed by initialAlignment.cpp
	{
		if(i== 0)//construct initial cell (root of traceback tree)
		{
			//make fist cell of first column			
			pair<int, CellPtr> first(i, CellPtr(new Cell(null_distance, null_previous)));
			distanceMatrix.insert(first);
			//first cell is root of traceback
			pair<int, int> root_cell(i,i);
			trace_back.push(root_cell);
				
		}//end of if			
			
		else 
		{
			//Make pair to be inserted
			pair<int, CellPtr> diag_cell(i, CellPtr(new Cell(max_distance, null_previous)));
			distanceMatrix.insert(diag_cell);
					
			//compute distance for no merge
			float previousD = 0.0f;
			if(distanceMatrix[i-1]!= 0)
			{previousD = distanceMatrix[i-1]->getDistance();}
			else{previousD = MAXD;}
		
			int label_of_distance = (10 * i) + 1;//label of precomputed distance
			if(distances.find(label_of_distance)!= distances.end())
			{distance_1 = previousD + distances.find(label_of_distance)->second;}
			else{distance_1 = previousD;}//if current distance is not found => no candidate for merge => distance of no merge = 0
	
			if(i > 1) //merge 2-1
			{
				//compute distance for merge 2-1
				float previousD = 0.0f;
				if(distanceMatrix[i-2] != 0)
				{previousD = distanceMatrix[i-2]->getDistance();}
				else{previousD = MAXD;}

				int label_of_distance = (10 * i) + 2;//label of precomputed distance
				if(distances.find(label_of_distance)!= distances.end())
				{distance_2 = previousD + distances.find(label_of_distance)->second;}
				else{distance_2 = MAXD;}//if current distance not found => no candidate for merge => distance of merge = infinity
			}
			if(i > 2) //possibility for merge 3-1
			{
				//compute distance for merge 3-1
				float previousD = 0.0f;
				if(distanceMatrix[i-3] != 0)
				{previousD = distanceMatrix[i-3]->getDistance();}
				else{previousD = MAXD;}

				int label_of_distance = (10 * i) + 3;//label of precomputed distance
				if(distances.find(label_of_distance)!= distances.end())
				{distance_3 = previousD + distances.find(label_of_distance)->second;}
				else{distance_3 = MAXD;}//if current distance not found => no candidate for merge => distance of merge = infinity
			}
			if(i > 3) //possibility for merge 4-1
			{
				//compute distance for merge 4-1
				float previousD = 0.0f;
				if(distanceMatrix[i-4] != 0)
				{previousD = distanceMatrix[i-4]->getDistance();}
				else{previousD = MAXD;}

				int label_of_distance = (10 * i) + 4;//label of precomputed distance
				if(distances.find(label_of_distance)!= distances.end())
				{distance_4 = previousD + distances.find(label_of_distance)->second;}
				else{distance_4 = MAXD;}//if current distance not found => no candidate for merge => distance of merge = infinity
			}
	
			//look for minimal distance
			long double d_min = distance_1;

			if(distance_2 < d_min)
			{			
				d_min = distance_2;
			}

			if(distance_3 < d_min)
			{
				d_min = distance_3;
			}

			if(distance_4 < d_min)
			{
				d_min = distance_4;
			}

			//set pointer to previous cell
			if(d_min == distance_1) //no merge
			{
				distanceMatrix[i]-> setDistance(distance_1);
				pair<int, int> previous;
				previous.first = i-1;
				previous.second = i-1; 
				distanceMatrix[i]-> setPrevious(previous);				
			}
			else if(d_min == distance_2) //merge 2-1	
			{
				distanceMatrix[i]-> setDistance(distance_2);
				pair<int, int> previous;
				previous.first = i-2;
				previous.second = i-2;
				distanceMatrix[i]-> setPrevious(previous);
			}
			else if(d_min == distance_3)//merge 3-1
			{
				distanceMatrix[i]-> setDistance(distance_3);
				pair<int, int> previous;
				previous.first = i-3;
				previous.second = i-3;
				distanceMatrix[i]-> setPrevious(previous);
			}
			else if(d_min == distance_4)//merge 4-1
			{
				distanceMatrix[i]-> setDistance(distance_4);
				pair<int, int> previous;
				previous.first = i-4;
				previous.second = i-4;
				distanceMatrix[i]-> setPrevious(previous);
			}
		}//end of else
	}//end of loop 

//3. Trace back 
	//begin at last cell
	int size_of_row = (int) a.size();
	pair<int, int> current_cell(size_of_row, size_of_row);

	//end at root cell
	pair<int,int> root_cell(0,0);

	//store found cells in BackTrack temp
	stack< pair<int, int> > back_track_temp;


	while(!(current_cell.first == root_cell.first && current_cell.second == root_cell.second))
	{	
		trace_back.push(current_cell);
		pair<int,int> previous_cell;		  						
		
		if(distanceMatrix[current_cell.first] != NULL) 
	    	{
			previous_cell = distanceMatrix[current_cell.first]-> getPrevious();
		}
	
		else 
		{
			cout  << "No path has been found in DP procedure. Return empty alignment." << endl;
			stack<Chunk> empty_alignment;
			setExpandedAlignment(empty_alignment);
			return;
		}				
		//set current cell to previous cell
		current_cell = previous_cell;
	}
//clear distance map
distances.clear();


// 4.Merge Chunks that are in traceback

	//offset positions of source and target chunks
	vector<long int> nullP;
	vector<long int> merged_source_chunk_position = nullP;
	vector<long int> merged_target_chunk_position = nullP;
	vector< vector<long int> > merged_source_chunk_token = nullC;
	vector< vector<long int> > merged_target_chunk_token = nullC;


	//initialize pass counter
	pass_counter = 0;
	
	//loop through alignment and merge chunks
	while(!reverse_alignment_2.empty())
	{
		Chunk current = reverse_alignment_2.top();
		reverse_alignment_2.pop();

		//get cell in trace-back
		pair<int,int> traced = trace_back.top();			
		trace_back.pop();
		
		pass_counter++;

		//no merge
		if(traced.first == pass_counter)
		{
			expanded_alignment_temp.push(current);
		}

		//merge into 1-2/2-1
		else if(traced.first == pass_counter + 1)
		{
			int first_position = traced.first -2; //vector begins at 0
			int second_position = traced.first -1;
			Chunk merged_chunk(merged_source_chunk_position, merged_target_chunk_position, merged_source_chunk_token, merged_target_chunk_token);

			vector<long int> first_source_chunk_position = first_alignment.at(first_position).getSourceChunkPosition();
			vector<long int> first_target_chunk_position = first_alignment.at(first_position).getTargetChunkPosition();
			vector<long int> second_source_chunk_position = first_alignment.at(second_position).getSourceChunkPosition();
			vector<long int> second_target_chunk_position = first_alignment.at(second_position).getTargetChunkPosition();

			vector<long int> :: iterator itr_chunk_pos;

			//put merged positions in merged chunk
			for(itr_chunk_pos = first_source_chunk_position.begin(); itr_chunk_pos != first_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}
		
			for(itr_chunk_pos = second_source_chunk_position.begin(); itr_chunk_pos != second_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}	

			for(itr_chunk_pos = first_target_chunk_position.begin(); itr_chunk_pos != first_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}				
			
			for(itr_chunk_pos = second_target_chunk_position.begin(); itr_chunk_pos != second_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}

			expanded_alignment_temp.push(merged_chunk);
			pass_counter = traced.first;
			reverse_alignment_2.pop();

			merged_chunk.clear();

			first_source_chunk_position.clear();
			first_target_chunk_position.clear();
			
			second_source_chunk_position.clear();
			second_target_chunk_position.clear();
			
			merged_source_chunk_position.clear();
			merged_target_chunk_position.clear();
			
		}

		//merge into 1-3/3-1
		else if(traced.first == pass_counter + 2)
		{

			int first_position = traced.first -3;
			int second_position = traced.first -2;
			int third_position = traced.first -1;

			Chunk merged_chunk(merged_source_chunk_position, merged_target_chunk_position, merged_source_chunk_token, merged_target_chunk_token);

			vector<long int> first_source_chunk_position = first_alignment.at(first_position).getSourceChunkPosition();
			vector<long int> first_target_chunk_position = first_alignment.at(first_position).getTargetChunkPosition();
			vector<long int> second_source_chunk_position = first_alignment.at(second_position).getSourceChunkPosition();
			vector<long int> second_target_chunk_position = first_alignment.at(second_position).getTargetChunkPosition();

			vector<long int> third_source_chunk_position = first_alignment.at(third_position).getSourceChunkPosition();
			vector<long int> third_target_chunk_position = first_alignment.at(third_position).getTargetChunkPosition();

			vector<long int> :: iterator itr_chunk_pos;

			//put merged positions in merged chunk
			for(itr_chunk_pos = first_source_chunk_position.begin(); itr_chunk_pos != first_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}

			for(itr_chunk_pos = second_source_chunk_position.begin(); itr_chunk_pos != second_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}	

			for(itr_chunk_pos = third_source_chunk_position.begin(); itr_chunk_pos != third_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}	
		
			for(itr_chunk_pos = first_target_chunk_position.begin(); itr_chunk_pos != first_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}	
		
			for(itr_chunk_pos = second_target_chunk_position.begin(); itr_chunk_pos != second_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}			
			
			for(itr_chunk_pos = third_target_chunk_position.begin(); itr_chunk_pos != third_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}

			expanded_alignment_temp.push(merged_chunk);
			pass_counter = traced.first;
			reverse_alignment_2.pop();
			reverse_alignment_2.pop();

			merged_chunk.clear();

			first_source_chunk_position.clear();
			first_target_chunk_position.clear();
			
			second_source_chunk_position.clear();
			second_target_chunk_position.clear();

			third_source_chunk_position.clear();
			third_target_chunk_position.clear();

			merged_source_chunk_position.clear();
			merged_target_chunk_position.clear();
		}

		//merge into 1-to-4/4-to-1
		else if(traced.first == pass_counter + 3)
		{
			int first_position = traced.first -4;
			int second_position = traced.first -3;
			int third_position = traced.first -2;
			int fourth_position = traced.first -1;

			Chunk fourth_chunk = first_alignment.at(fourth_position);
			Chunk merged_chunk(merged_source_chunk_position, merged_target_chunk_position, merged_source_chunk_token, merged_target_chunk_token);

			vector<long int> first_source_chunk_position = first_alignment.at(first_position).getSourceChunkPosition();
			vector<long int> first_target_chunk_position = first_alignment.at(first_position).getTargetChunkPosition();
			
			vector<long int> second_source_chunk_position = first_alignment.at(second_position).getSourceChunkPosition();
			vector<long int> second_target_chunk_position = first_alignment.at(second_position).getTargetChunkPosition();

			vector<long int> third_source_chunk_position = first_alignment.at(third_position).getSourceChunkPosition();
			vector<long int> third_target_chunk_position = first_alignment.at(third_position).getTargetChunkPosition();

			vector<long int> fourth_source_chunk_position = first_alignment.at(fourth_position).getSourceChunkPosition();
			vector<long int> fourth_target_chunk_position = first_alignment.at(fourth_position).getTargetChunkPosition();

			vector<long int> :: iterator itr_chunk_pos;

			//put merged positions in merged chunk
			for(itr_chunk_pos = first_source_chunk_position.begin(); itr_chunk_pos != first_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}

			for(itr_chunk_pos = second_source_chunk_position.begin(); itr_chunk_pos != second_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}

			for(itr_chunk_pos = third_source_chunk_position.begin(); itr_chunk_pos != third_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}	

			for(itr_chunk_pos = fourth_source_chunk_position.begin(); itr_chunk_pos != fourth_source_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToSourceChunkPosition(position);}
			}		
		
			for(itr_chunk_pos = first_target_chunk_position.begin(); itr_chunk_pos != first_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}	
		
					
			for(itr_chunk_pos = second_target_chunk_position.begin(); itr_chunk_pos != second_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}
			
			
			for(itr_chunk_pos = third_target_chunk_position.begin(); itr_chunk_pos != third_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}
			
			
			for(itr_chunk_pos = fourth_target_chunk_position.begin(); itr_chunk_pos != fourth_target_chunk_position.end(); itr_chunk_pos++ )
			{
				long int position = *itr_chunk_pos;
				if(position != -1) // deletions and insertions are marked by -1, if merging do not add the empty position -1
				{merged_chunk.addToTargetChunkPosition(position);}
			}

			expanded_alignment_temp.push(merged_chunk);
			pass_counter = traced.first;
			reverse_alignment_2.pop();
			reverse_alignment_2.pop();	
			reverse_alignment_2.pop();

			merged_chunk.clear();

			first_source_chunk_position.clear();
			first_target_chunk_position.clear();
			
			second_source_chunk_position.clear();
			second_target_chunk_position.clear();

			third_source_chunk_position.clear();
			third_target_chunk_position.clear();

			fourth_source_chunk_position.clear();
			fourth_target_chunk_position.clear();

			merged_source_chunk_position.clear();
			merged_target_chunk_position.clear();
		}//end of else		
	}//end of while

setExpandedAlignment(expanded_alignment_temp);
}

