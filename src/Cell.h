#ifndef CELL_H_
#define CELL_H_

#include <utility> 

using namespace std;

class Cell //cells of the distance matrix of the DP procedure
{
	private:
	
	long double distance;
	pair<int,int> previous;

	public:
	Cell();
	Cell(long double d, pair<int,int> p);
	
	long double getDistance();
	void setDistance(long double d);
	
	pair<int,int> getPrevious();
	void setPrevious (pair<int,int> p);
};

#endif /*CELL_H_*/
