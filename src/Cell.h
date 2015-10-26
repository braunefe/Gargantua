#ifndef CELL_H_
#define CELL_H_

#include <utility>

using namespace std;

class Cell  // cells of the distance matrix of the DP procedure
    {
 private:
  long double distance;
  pair<int, int> previous;
  bool initialized;

 public:
  Cell() : distance(0), initialized(false){};
  Cell(long double d, pair<int, int> p)
      : distance(d), previous(p), initialized(true){};

  long double getDistance() const;
  void setDistance(long double d);

  pair<int, int> getPrevious() const;
  void setPrevious(pair<int, int> p);
};

#endif /*CELL_H_*/
