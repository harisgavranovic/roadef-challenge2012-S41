#include "process.h"


Process::Process(int id,  int move_cost, Service* service) :
    	id_(id), move_cost_(move_cost),
    	service_(service),
    	machine_(0), initial_machine_(0),
    	position_abs_requirements_(id)
{
	if(service_ != 0) service_->addProcess(this);
}


void Process::assign(Machine* m)
{
	if(machine_ == m) return;

	service_->updateWithProcessReassign(this, machine_, m);

	if(machine_ != 0) machine_->moveOut(this);

	if(m !=0 )    m->moveIn (this);

	machine_ = m;

}

void Process::assignInitial(Machine* m)
{
	service_->updateWithProcessReassign(this, 0, m);
    initial_machine_ = m;
	machine_ = m;
	m->moveInInitial (this);
}


int64 Process::getTotalRequirement()
{
	int64 totalReq = 0;
	for(vector<int64>::iterator it = requirements_.begin(); it != requirements_.end(); it++)
		totalReq += (*it);
	return totalReq;
}

int Process::getMachineId()
{
	return machine_->getId();
}

Location* Process::getLocation()
{
	return machine_->getLocation();
}

Neighborhood* Process::getNeighborhood()
{
	return machine_->getNeighborhood();
}

int Process::getLocationId()
{
	return machine_->getLocation()->getId();
}

int Process::getNeighborhoodId()
{
	return machine_->getNeighborhood()->getId();
}


int Process::getPosition() {
		 return position_abs_requirements_ ;
}

