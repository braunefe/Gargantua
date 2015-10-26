#ifndef LENGTHDISTRIBUTIONS_H_
#define LENGTHDISTRIBUTIONS_H_

#include <vector>
#include <utility>
#include <cmath>
#include "boost/math/distributions/poisson.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/string.hpp"

#include"Reader.h"
#include"Data.h"
#include"Sentence.h"


using namespace std;

class LengthDistributions
{	
	friend class boost::serialization::access;

	public : 

	float param_of_poisson;

	map<int, float> relative_frequencies_of_source;
	map<int, float> relative_frequencies_of_target;


	template<class Archive>
    	void serialize(Archive & ar, const unsigned int version)
    	{
		ar & param_of_poisson;
		
        	ar & relative_frequencies_of_source;
        	ar & relative_frequencies_of_target;		
	}

	LengthDistributions() : param_of_poisson(1.0) {};
	LengthDistributions(float param_of_poisson, map<int, float> rfs, map<int, float> rft);

	float getParamOfPoisson() const;
	void setParamOfPoisson(float p);

	map<int, float> getRelativeFrequenciesOfSource();
	void setRelativeFrequenciesOfSource(map<int, float> rfs);

	map<int, float> getRelativeFrequenciesOfTarget();
	void setRelativeFrequenciesOfTarget(map<int, float> rft);
	
	//Estimates mean of poisson distribution for sentence lengths
	void estimateParamOfPoisson(string input_path_source, string input_path_target);
	
	//Computes the raw relative frequencies of source and target sentence lengths
	void computeRawRelativeFrequencies(string input_path_source, string input_path_target);

	//computes poisson log probability for source and target sentence lenghts l1 and l2
	float distanceMeasurePoiss(int l1, int l2) const;

	//for debugging
	//void writeRelativeFrequencies();

};


#endif /*LENGTHDISTRIBUTIONS_H_*/
