#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_

#include <vector>
using namespace std;

#include "machine.h"

class Machine;

class Neighborhood {

 private:

	int id_;

	vector<Machine*> machines_;

 public:

	Neighborhood(int id) : id_(id) {}

	int getId() {return id_;}

	void addMachine(Machine* m) { machines_.push_back(m); }

	Machine* getMachine(int i) { return machines_.at(i); }

	int getNumberOfMachines() {return machines_.size(); }
};



#endif /* NEIGHBORHOOD_H_ */
