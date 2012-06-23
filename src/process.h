#ifndef PROCESS_H_
#define PROCESS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "assert.h"
using namespace std;

#include "machine.h"
class Machine;

#include "service.h"
class Service;

#include "location.h"
#include "neighborhood.h"

class Location;
class Neighborhood;


class Process {

 private:

    int  id_;
    Service* service_;
    Machine*  machine_;
    Machine*  initial_machine_;
    int64 move_cost_;
    vector<int64> requirements_;

    int position_abs_requirements_;

 public:

    Process(int id,  int move_cost, Service* service) ;

    void addRequirement(int64 req) { requirements_.push_back(req); }

    void setRequirement(int r, int64 req) { requirements_[r] = req; }


	void assign(Machine* m) ;
	void assignInitial(Machine* m) ;

	void setAbsPosition(int pos) {position_abs_requirements_ = pos;}
	int getPosition();

    int  getId()           { return id_;        }
    int  getMoveCost()     { return move_cost_; }
    Service* getService()  { return service_;   }
    Machine* getMachine()  {return machine_;    }

    Machine* getInitialMachine()  { return initial_machine_; }

    vector<int64> getRequirements()     { return requirements_;       }
    int64         getRequirement(int r) { return requirements_.at(r); }

    int64 getTotalRequirement() ;
    double getRelativeRequirement();

    int64 getProcessMoveCost() {
    	if(machine_ != initial_machine_) return move_cost_;
    }


    int getMachineId() ;
    Location* getLocation() ;
    Neighborhood* getNeighborhood() ;
    int getLocationId() ;
    int getNeighborhoodId() ;


};



#endif /* PROCESS_H_ */
