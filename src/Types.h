/*
 * Types.h
 *
 *  Created on: 25 Oct 2015
 *      Author: buck
 */

#ifndef TYPES_H_
#define TYPES_H_


#include <set>
#include <map>
#include "boost/unordered_map.hpp"
#include <google/sparse_hash_map>

typedef boost::unordered_map<long int, float> WordProbs;
typedef boost::unordered_map<long int, WordProbs > LexProbs;




#endif /* TYPES_H_ */
