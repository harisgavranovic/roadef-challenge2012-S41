#ifndef MACHINE_H_
#define MACHINE_H_

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <set>
using namespace std;

#include "types.h"
#include "resource.h"
#include "balance.h"
#include "machine_resource.h"
#include "service.h"


class Process;
class Service;
class Data;
class MachineResource;


#include "location.h"
#include "neighborhood.h"

class Neighborhood;
class Location;

class Machine {

 private:

   int id_;

   vector<Process*> processes_;
   vector<MachineResource*> machine_resources_;

   Location* location_;
   Neighborhood* neighborhood_;

   vector<int64> to_machine_move_costs_;

   Data* data_;

 public:

   // number of machine resources with free safety capacity
   int number_of_resources_with_positive_safety_remaining_capacity_;
   vector<MachineResource*> machine_resources_sorted_;

 public:

   Machine(int id, Data* data);

   Machine(int id) : id_(id) {}

   void addMachineResource(MachineResource* m_r) {
	   machine_resources_.push_back(m_r);
	   machine_resources_sorted_.push_back(m_r);
   }

   void addToMachineMoveCost(int64 cost)         {to_machine_move_costs_.push_back(cost);}

   void setId(int id) { id_ = id; }

   void setLocation(Location* loc);
   void setNeighborhood(Neighborhood* hood);

   int getId() {return id_;}

   Location* getLocation()     { return location_;     }
   Neighborhood* getNeighborhood() { return neighborhood_; }

   int getLocationId()     ;
   int getNeighborhoodId() ;

   vector<Process*>& getProcesses()    { return processes_;         }

   vector<Process*> getProcessesCopy() { return processes_;         }

   Process* getProcess(int i)     { return processes_.at(i);     }
   int  getNumberOfProcesses()    { return processes_.size();    }

   MachineResource* getMachineResource(int resource_id) {return machine_resources_.at(resource_id);}
   MachineResource* getMachineResource(Resource* resource);

   int64 getLoadCost() ;
   int64 getBalanceCost(Balance* balance) ;
   int64 getBalanceCost() ;

   int64 getLoadCostForResource(int r);

   void moveIn(Process* process);
   void moveInInitial(Process* process);
   void moveOut(Process* process);

   int64 getToMachineMoveCost(Machine* m)   {
	   if(m == 0) return 1;
	   return to_machine_move_costs_[m->getId()];
   }

   int64 getToMachineMoveCost(int m_id)     {return to_machine_move_costs_[m_id];}

   bool hasNegativeRemainingCapacity();
   bool getNumberOfNegativeRemainingCapacities();

   bool checkCapacityWithAddingProcess(Process* p, int* sorted_resources);
   bool checkCapacityWithSwap(Process* p1, Process* p2);

   int64 getLoadCostDifferenceWithRemovingProcess(Process* p);
   int64 getLoadCostDifferenceWithAddingProcess(Process* p);

   int64 getBalanceCostDifferenceWithRemovingProcess(Process* p, Balance* b);
   int64 getBalanceCostDifferenceWithAddingProcess(Process* p, Balance* b);
   int64 getBalanceCostDifferenceWithRemovingProcess(Process* p);
   int64 getBalanceCostDifferenceWithAddingProcess(Process* p);

   int64 getLoadAndBalanceCostDifferenceWithRemovingProcess(Process* p);
   int64 getLoadAndBalanceCostDifferenceWithAddingProcess(Process* p);


   int64 getCostDifferenceWithRemovingProcess(Process* p) {
     	return getLoadCostDifferenceWithRemovingProcess(p) + getBalanceCostDifferenceWithRemovingProcess(p);
   }

   int64 getCostDifferenceWithAddingProcess(Process* p) {
    	return getLoadCostDifferenceWithAddingProcess(p) + getBalanceCostDifferenceWithAddingProcess(p);
   }

   int64 getLoadCostDifferenceWithSwap(Process* p1, Process* p2);
   int64 getBalanceCostDifferenceWithSwap(Process* p1, Process* p2);
   int64 getBalanceCostDifferenceWithSwap(Process* p1, Process* p2, Balance* b);


   int64 getCostDifferenceWithSwap(Process* p1, Process* p2) {
	   return getLoadCostDifferenceWithSwap(p1, p2) +
	          getBalanceCostDifferenceWithSwap(p1, p2);
   }

   Data* getData() {return data_;}

};



#endif /* MACHINE_H_ */
