#ifndef MACHINE_RESOURCE_H_
#define MACHINE_RESOURCE_H_


#include "machine.h"
class Machine;

class Process;

class MachineResource {

private:

	Machine*  machine_;
	Resource* resource_;

	int64 capacity_;
	int64 safety_capacity_;
	int64 remaining_capacity_;
	int64 remaining_safety_capacity_;
	int64 remaining_transient_capacity_;
	int64 remaining_transient_capacity_in_initial_solution_;

public:

	MachineResource() : machine_(0), resource_(0){}

	MachineResource(Machine* m, Resource* r) : machine_(m), resource_(r) {}

	void setCapacity(int64 cap) ;
	void setSafetyCapacity(int64 cap) ;

	Machine*  getMachine()  { return machine_;  }
	Resource* getResource() { return resource_; }

    int64 getCapacity() {return capacity_;}
    int64 getSafetyCapacity() {return safety_capacity_;}
    int64 getRemainingCapacity() {return remaining_capacity_;}
    int64 getRemainingSafetyCapacity() {return remaining_safety_capacity_;}
    int64 getTransientRemainingCapacity() {return remaining_transient_capacity_;}
    int64 getTransientRemainingCapacityInInitialSolution() {
    	return remaining_transient_capacity_in_initial_solution_;
    }

	int64 getLoadCost() ;

	void consume(Process* process, bool consumeTR = true);
	void undoConsumption(Process* process, bool freeTR = true);
	void consumeInitial(Process* process);

	bool checkCapacityWithAddingProcess(Process* p);
	bool checkCapacityWithSwap(Process* p1, Process* p2);

	int64 getLoadCostDifferenceWithRemovingProcess(Process* p);
	int64 getLoadCostDifferenceWithAddingProcess(Process* p);

	int64 getLoadCostDifferenceWithSwap(Process* p1, Process* p2);

	void decreaseCapacity(int64 d);
	void decreaseSafetyCapacity(int64 d);
};


#endif /* MACHINE_RESOURCE_H_ */
