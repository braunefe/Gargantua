#include <utility> 
#include "Cell.h"

Cell::Cell(long double d, pair<int,int> p)
{
	setDistance(d);
	setPrevious(p);
}

long double Cell::getDistance()
{
	return distance;	
}

void Cell::setDistance(long double d)
{
	distance = d;
}

pair<int,int> Cell::getPrevious()
{
	return previous;
}

void Cell::setPrevious(pair<int,int> p)
{
	previous = p;
}
