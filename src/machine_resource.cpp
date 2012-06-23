#include "machine_resource.h"


void MachineResource::setCapacity(int64 cap)
{
	capacity_ = cap;
	remaining_capacity_ = cap;
	remaining_transient_capacity_ = cap;
	remaining_transient_capacity_in_initial_solution_ = cap;
}

void MachineResource::setSafetyCapacity(int64 s_cap)
{
	safety_capacity_ = s_cap;
	remaining_safety_capacity_ = s_cap;
}

void MachineResource::decreaseCapacity(int64 d)
{
	capacity_ -= d;
	if(!resource_->is_transient)
	  remaining_transient_capacity_in_initial_solution_ -= d;
}

void MachineResource::decreaseSafetyCapacity(int64 d)
{
	safety_capacity_ -= d;
}


int64 MachineResource::getLoadCost()
{

	if(remaining_safety_capacity_ < 0)
	{
		return (-1) * remaining_safety_capacity_ * resource_->load_cost_weight;
	}
	return 0l;

}


void MachineResource::consume(Process* process, bool consumeTR)
{

	remaining_capacity_ -= process->getRequirement(resource_->id_);
	remaining_safety_capacity_ -= process->getRequirement(resource_->id_);

	if(!resource_->is_transient || consumeTR)
	  remaining_transient_capacity_ -= process->getRequirement(resource_->id_);

	if(remaining_safety_capacity_ + process->getRequirement(resource_->id_) >= 0 &&
	   remaining_safety_capacity_ < 0)
	{
		machine_->number_of_resources_with_positive_safety_remaining_capacity_ =
				machine_->number_of_resources_with_positive_safety_remaining_capacity_ - 1;
	}

}


void MachineResource::consumeInitial(Process* process)
{
	remaining_capacity_ -= process->getRequirement(resource_->id_);
	remaining_safety_capacity_ -= process->getRequirement(resource_->id_);
    remaining_transient_capacity_ -= process->getRequirement(resource_->id_);
	if(resource_->is_transient)
      remaining_transient_capacity_in_initial_solution_ = remaining_transient_capacity_;

	if(remaining_safety_capacity_ + process->getRequirement(resource_->id_) >= 0 &&
	   remaining_safety_capacity_ < 0)
	{
		machine_->number_of_resources_with_positive_safety_remaining_capacity_ =
				machine_->number_of_resources_with_positive_safety_remaining_capacity_ - 1;
	}

}



void MachineResource::undoConsumption(Process* process, bool freeTR)
{

	remaining_capacity_ += process->getRequirement(resource_->id_);
	remaining_safety_capacity_ += process->getRequirement(resource_->id_);

	if(!resource_->is_transient || freeTR)
	  remaining_transient_capacity_ += process->getRequirement(resource_->id_);


	if(remaining_safety_capacity_ - process->getRequirement(resource_->id_) < 0 &&
	   remaining_safety_capacity_ >= 0)
	{
		machine_->number_of_resources_with_positive_safety_remaining_capacity_ =
				machine_->number_of_resources_with_positive_safety_remaining_capacity_ + 1;
	}


}


bool MachineResource::checkCapacityWithAddingProcess(Process* p)
{
	 if(resource_->is_transient && p->getInitialMachine() == machine_) return true;
	 if( remaining_transient_capacity_ - p->getRequirement(resource_->id_) < 0 ) return false;
     return true;
}



bool MachineResource::checkCapacityWithSwap(Process* p1, Process* p2)
{
    int64 new_remaining_transient = remaining_transient_capacity_;

    if(!resource_->is_transient || p2->getInitialMachine() != machine_)
	   new_remaining_transient -= p2->getRequirement(resource_->id_);

	if( new_remaining_transient >= 0)
		return true;

    if(!resource_->is_transient || p1->getInitialMachine() != machine_)
    	new_remaining_transient += p1->getRequirement(resource_->id_);

	if( new_remaining_transient < 0)
		return false;

	return true;
}


int64 MachineResource::getLoadCostDifferenceWithRemovingProcess(Process* p)
{

  return
  (
	max(0LL, (-1) * (remaining_safety_capacity_ + p->getRequirement(resource_->id_)))
  - max(0LL, (-1) * remaining_safety_capacity_)
  )
  * resource_->load_cost_weight;
}

int64 MachineResource::getLoadCostDifferenceWithAddingProcess(Process* p)
{

  return
  (
	max(0LL, (-1) * (remaining_safety_capacity_ - p->getRequirement(resource_->id_)))
	- max(0LL, (-1) * remaining_safety_capacity_)
  )
  * resource_->load_cost_weight;
}


int64 MachineResource::getLoadCostDifferenceWithSwap(Process* p1, Process* p2)
{

  return
  (
	max(0LL, (-1) * (remaining_safety_capacity_ +
			         p1->getRequirement(resource_->id_) -
			         p2->getRequirement(resource_->id_)))
	- max(0LL, (-1) * remaining_safety_capacity_)
  )
  * resource_->load_cost_weight;
}




