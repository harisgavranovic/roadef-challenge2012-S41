#include "machine.h"


Machine::Machine(int id, Data* data) : id_(id), data_(data) {}


MachineResource* Machine::getMachineResource(Resource* resource)
{
	  for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
	  {
		  if((*it)->getResource() == resource) return (*it);
	  }
	   return 0;
}

void Machine::setLocation(Location* loc)  {
	   location_ = loc;
	   location_->addMachine(this);
}

void Machine::setNeighborhood(Neighborhood* hood) {
	   neighborhood_ = hood;
	   neighborhood_->addMachine(this);
}

int64 Machine::getLoadCost()
{
	int64 load_cost = 0;
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
		load_cost += (*it)->getLoadCost();
    return load_cost;
}

int64 Machine::getLoadCostForResource(int r)
{
    return machine_resources_[r]->getLoadCost();
}


int64 Machine::getBalanceCost(Balance* balance)
{
	int64 remaining_on_target =
	  balance->target *
	  machine_resources_.at(balance->first_resource_id)->getRemainingCapacity();
	int64 remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity();
	return balance->weight * max(0LL, remaining_on_target - remaining);
}

int64 Machine::getBalanceCost()
{
	int64 balance_cost = 0;
	for(int b = 0; b < data_->getNumberOfBalances(); b++)
	    balance_cost += getBalanceCost(data_->getBalance(b));
    return balance_cost;
}




bool smallerRemainingCapacity(MachineResource* mr1, MachineResource* mr2)
{
	return mr1->getRemainingCapacity() < mr2->getRemainingCapacity();
}




void Machine::moveIn(Process* process)
{

	if(process->getMachine() == this)
		throw ("machine.cpp: cannon move on the same machine");

	processes_.push_back(process);

	bool consumeTR = true;
    if(process->getInitialMachine() == this) consumeTR = false;

	// update remaining capacities
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
    	 (*it)->consume(process, consumeTR);


    sort(machine_resources_sorted_.begin(), machine_resources_sorted_.end(), smallerRemainingCapacity);

}


void Machine::moveInInitial(Process* process)
{
	processes_.push_back(process);

	// update remaining capacities
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
	  (*it)->consumeInitial(process);

    sort(machine_resources_sorted_.begin(), machine_resources_sorted_.end(), smallerRemainingCapacity);

}


void Machine::moveOut(Process* process)
{
	// remove process from the list
	vector<Process*>::iterator it;
	for (it = processes_.begin(); (*it) != process; it++);
    processes_.erase(it);

    bool freeTR = true;
    if(process->getInitialMachine() == this) freeTR = false;

    // update remaining capacities
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
    	(*it)->undoConsumption(process, freeTR);

    sort(machine_resources_sorted_.begin(), machine_resources_sorted_.end(), smallerRemainingCapacity);

}


bool Machine::hasNegativeRemainingCapacity()
{
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
    {
	  if((*it)->getTransientRemainingCapacity() < 0) return true;
    }
    return false;
}


bool Machine::getNumberOfNegativeRemainingCapacities()
{
	int nmb = 0;
    for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
	  if((*it)->getTransientRemainingCapacity() < 0) nmb++;
    return nmb;
}



//resources sorted by remaining capacity
bool Machine::checkCapacityWithAddingProcess(Process* p, int* sorted_resources)
{
	for(vector<MachineResource*>::iterator it = machine_resources_sorted_.begin(); it != machine_resources_sorted_.end(); it++)
	  if(!(*it)->checkCapacityWithAddingProcess(p))
		  return false;
	return true;
}


// check capacity constraints when p1
// removed from machine and p2 added to machine
bool Machine::checkCapacityWithSwap(Process* p1, Process* p2)
{
	for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
	  if(!(*it)->checkCapacityWithSwap(p1, p2)) return false;
	return true;
}



int64 Machine::getLoadCostDifferenceWithRemovingProcess(Process* p)
{
	int64 cost_difference = 0;
	for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
		cost_difference += (*it)->getLoadCostDifferenceWithRemovingProcess(p);
	return cost_difference;
}

int64 Machine::getLoadCostDifferenceWithAddingProcess(Process* p)
{
	int64 cost_difference = 0;
	for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
		cost_difference += (*it)->getLoadCostDifferenceWithAddingProcess(p);
	return cost_difference;
}

int64 Machine::getBalanceCostDifferenceWithRemovingProcess(Process* p, Balance* balance)
{
	int64 remaining_on_target =
	  balance->target *
	  machine_resources_.at(balance->first_resource_id)->getRemainingCapacity();

	int64 remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity();

	int64 old_balance_cost = balance->weight * max(0LL, remaining_on_target - remaining);

	int64 new_remaining_on_target =
	  balance->target *
	  (machine_resources_.at(balance->first_resource_id)->getRemainingCapacity() +
	   p->getRequirement(balance->first_resource_id));

	int64 new_remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity() +
		p->getRequirement(balance->second_resource_id);

	int64 new_balance_cost = balance->weight *
			max(0LL, new_remaining_on_target - new_remaining);

	return new_balance_cost - old_balance_cost;

}


int64 Machine::getBalanceCostDifferenceWithAddingProcess(Process* p, Balance* balance)
{
	int64 remaining_on_target =
	  balance->target *
	  machine_resources_.at(balance->first_resource_id)->getRemainingCapacity();

	int64 remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity();

	int64 old_balance_cost = balance->weight * max(0LL, remaining_on_target - remaining);

	int64 new_remaining_on_target =
	  balance->target *
	  (machine_resources_.at(balance->first_resource_id)->getRemainingCapacity() -
	   p->getRequirement(balance->first_resource_id));

	int64 new_remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity() -
		p->getRequirement(balance->second_resource_id);

	int64 new_balance_cost = balance->weight *
			max(0LL, new_remaining_on_target - new_remaining);

	return new_balance_cost - old_balance_cost;

}

int64 Machine::getBalanceCostDifferenceWithRemovingProcess(Process* p)
{
	int64 balance_difference = 0;
	for(int b = 0; b < data_->getNumberOfBalances(); b++)
		balance_difference +=
	    		getBalanceCostDifferenceWithRemovingProcess(p, data_->getBalance(b));
    return balance_difference;
}

int64 Machine::getBalanceCostDifferenceWithAddingProcess(Process* p)
{
	int64 balance_difference = 0;
	for(int b = 0; b < data_->getNumberOfBalances(); b++)
		balance_difference +=
	    		getBalanceCostDifferenceWithAddingProcess(p, data_->getBalance(b));
    return balance_difference;
}




int64 Machine::getLoadAndBalanceCostDifferenceWithRemovingProcess(Process* p)
{

	return getLoadCostDifferenceWithRemovingProcess(p) +
           getBalanceCostDifferenceWithRemovingProcess(p);

}

int64 Machine::getLoadAndBalanceCostDifferenceWithAddingProcess(Process* p)
{

	 return getLoadCostDifferenceWithAddingProcess(p) +
	       getBalanceCostDifferenceWithAddingProcess(p);
}



int64 Machine::getLoadCostDifferenceWithSwap(Process* p1, Process* p2)
{
	int64 cost_difference = 0LL;

	for(vector<MachineResource*>::iterator it = machine_resources_.begin(); it != machine_resources_.end(); it++)
		cost_difference += (*it)->getLoadCostDifferenceWithSwap(p1, p2);
	return cost_difference;
}



int64 Machine::getBalanceCostDifferenceWithSwap(Process* p1, Process* p2, Balance* balance)
{
	int64 remaining_on_target =
	  balance->target *
	  machine_resources_.at(balance->first_resource_id)->getRemainingCapacity();

	int64 remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity();

	int64 old_balance_cost = balance->weight * max(0LL, remaining_on_target - remaining);

	int64 new_remaining_on_target =
	  balance->target *
	  (machine_resources_.at(balance->first_resource_id)->getRemainingCapacity() -
	   p2->getRequirement(balance->first_resource_id) +
	   p1->getRequirement(balance->first_resource_id));

	int64 new_remaining =
		machine_resources_.at(balance->second_resource_id)->getRemainingCapacity() -
		p2->getRequirement(balance->second_resource_id) +
		p1->getRequirement(balance->second_resource_id);

	int64 new_balance_cost = balance->weight *
			max(0LL, new_remaining_on_target - new_remaining);

	return new_balance_cost - old_balance_cost;

}

int64 Machine::getBalanceCostDifferenceWithSwap(Process* p1, Process* p2)
{

	int64 balance_difference = 0;
	for(int b = 0; b < data_->getNumberOfBalances(); b++)
		balance_difference +=
				getBalanceCostDifferenceWithSwap(p1, p2, data_->getBalance(b));
    return balance_difference;
}


int Machine::getLocationId()
{
	return location_->getId();
}

int Machine::getNeighborhoodId()
{
	return neighborhood_->getId();
}
