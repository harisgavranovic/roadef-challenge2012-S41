#ifndef LOCATION_H_
#define LOCATION_H_

#include <vector>
using namespace std;

#include "machine.h"

class Machine;

class Location {

 private:

	int id_;

    vector<Machine*>  machines_;


 public:

    Location(int id) : id_(id) {}

	int getId() {return id_;}

	void addMachine(Machine* m) { machines_.push_back(m); }

	Machine* getMachine(int i) { return machines_.at(i); }

	int getNumberOfMachines() {return machines_.size(); }

};


#endif /* LOCATION_H_ */
