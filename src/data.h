#ifndef DATA_H_
#define DATA_H_

#include "machine.h"
#include "process.h"
#include "location.h"
#include "neighborhood.h"
#include "service.h"
#include "balance.h"
#include "resource.h"

class Location;
class Neighborhood;
class Service;
class Machine;
class Process;
class Solution;

class Data {

 private:

	vector<Process*> processes_;
	vector<Service*> services_;
	vector<Machine*> machines_;
	vector<Location*> locations_;
	vector<Neighborhood*> neighborhoods_;
	vector<Balance*>  balances_;
	vector<Resource*> resources_;

    int process_move_cost_weight_;
    int service_move_cost_weight_;
    int machine_move_cost_weight_;

 public:

    Data() {};

    void addProcess(Process* p)            { processes_.push_back(p);     }
    void addService(Service* s)            { services_.push_back(s);      }
    void addMachine(Machine* m)            { machines_.push_back(m);      }

    void addLocation(Location* l)        ;
    void addNeighborhood(Neighborhood* n)  ;

    void addBalance(Balance* b)            { balances_.push_back(b);      }
    void addResource(Resource* r)          { resources_.push_back(r);     }

    void setProcessMoveCostWeight(int w) { process_move_cost_weight_ = w;}
    void setServiceMoveCostWeight(int w) { service_move_cost_weight_ = w;}
    void setMachineMoveCostWeight(int w) { machine_move_cost_weight_ = w;}

    int getProcessMoveCostWeight() { return process_move_cost_weight_ ; }
    int getServiceMoveCostWeight() { return service_move_cost_weight_ ; }
    int getMachineMoveCostWeight() { return machine_move_cost_weight_ ; }

	int getNumberOfProcesses()     { return processes_.size();     }
	int getNumberOfServices()      { return services_.size();      }
	int getNumberOfMachines()      { return machines_.size();      }
    int getNumberOfLocations()     { return locations_.size();     }
    int getNumberOfNeighborhoods() { return neighborhoods_.size(); }
    int getNumberOfBalances()      { return balances_.size();      }
    int getNumberOfResources()     { return resources_.size();     }

    Process*      getProcess(int id)      { return processes_.at(id);     }
    Service*      getService(int id)      { return services_.at(id);      }
    Machine*      getMachine(int id)      { return machines_.at(id);      }
    Location*     getLocation(int id)     { return locations_.at(id);     }
    Neighborhood* getNeighborhood(int id) { return neighborhoods_.at(id); }
    Balance*      getBalance(int id)      { return balances_.at(id);      }
    Resource*     getResource(int id)     { return resources_.at(id);     }

    vector<Process*> &      getProcesses() 		{ return processes_;     }
    vector<Service*> &      getServices()  		{ return services_;      }
    vector<Machine*> &      getMachines()       { return machines_;      }
    vector<Machine*>        getMachinesCopy()   { return machines_;      }
    vector<Location*> &     getLocations()      { return locations_;     }
    vector<Neighborhood*> & getNeighborhoods()  { return neighborhoods_; }
    vector<Balance*> &      getBalances()  		{ return balances_;      }
    vector<Resource*> &     getResources() 		{ return resources_;     }
    vector<Process*>        getProcessesCopy() 	{ return processes_;     }

    int64 getLoadCostLB(int r = -1);

    void calculateProcessesPositionsByRequirements();

};


#endif /* DATA_H_ */
