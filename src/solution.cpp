#include "solution.h"


Solution::Solution() :
	    data_(0),
		max_number_of_moved_processes_(0),
		number_of_services_with_max_number_of_moved_processes_(1),
		nmb_of_conflicted_machines_(0),
		nmb_of_services_with_small_spread_(0),
		nmb_of_machines_with_violated_capacity_(0),
	    load_cost_(0),
	    balance_cost_(0),
	    process_move_cost_(0),
	    machine_move_cost_(0),
	    best_cost_(100000000000000) {}



void Solution::setData(Data* data)
{

	data_ = data;

	int nmbServices = data->getNumberOfServices();
	int nmbMachines = data->getNumberOfMachines();
	nmb_processes_from_service_on_machine_ = new int*[nmbServices];
	for(int s = 0; s < nmbServices; s++)
		nmb_processes_from_service_on_machine_[s] = new int[nmbMachines];

	for(int s = 0; s < nmbServices; s++)
	 for(int m = 0; m < nmbMachines; m++)
		nmb_processes_from_service_on_machine_[s][m] = 0;


	int nmbProcesses = data->getNumberOfProcesses();
	int nmbResources = data->getNumberOfResources();
	process_requirements_sorted_ = new int*[nmbProcesses];
	for(int p = 0; p < nmbProcesses; p++)
		process_requirements_sorted_[p] = new int[nmbResources];

	for(int p = 0; p < nmbProcesses; p++)
	 for(int r = 0; r < nmbResources; r++)
		process_requirements_sorted_[p][r] = r;

	if(data->getNumberOfProcesses() * data->getNumberOfMachines() < 2000 * 20000)
	{
		shift_matrix_ = new bool*[nmbMachines];
		for(int m = 0; m < nmbMachines; m++)
			shift_matrix_[m] = new bool[nmbProcesses];


		cost_difference_matrix_ = new int64*[nmbMachines];
		for(int m = 0; m < nmbMachines; m++)
			cost_difference_matrix_[m] = new int64[nmbProcesses];
	}

}


void Solution::fillMatrix()
{

	matrix_ = new bool*[processes_.size()];
	for(int i = 0; i < processes_.size(); i++)
		matrix_[i] = new bool[machines_.size()];

    for(int p = 0; p < processes_.size(); p++) {
		for(int m = 0; m < machines_.size(); m++)
		{
		   bool can_assign_process = true;
		   for(int r = 0; r < data_->getNumberOfResources(); r++)
		   {
			 if(processes_[p]->getRequirement(r) >
			    machines_[m]->getMachineResource(r)->getTransientRemainingCapacityInInitialSolution())
						 can_assign_process = false;
		   }
		   matrix_[p][m] = can_assign_process;
		}
		matrix_[p][processes_[p]->getInitialMachine()->getId()] = true;
    }

}


void Solution::addProcess(Process* p)
{
	processes_.push_back(p);


	// sort requirements for process
	int R = data_->getNumberOfResources();
	int process_index = p->getId();
	while(true)
	{
		bool swapped = false;
		for (int i = 0 ; i < R - 1; i++)
		{
		   if (p->getRequirement(process_requirements_sorted_[process_index][i + 1]) >
		       p->getRequirement(process_requirements_sorted_[process_index][i]) )
		   {
			   int x = process_requirements_sorted_[process_index][i];
			   process_requirements_sorted_[process_index][i] = process_requirements_sorted_[process_index][i + 1];
			   process_requirements_sorted_[process_index][i + 1] = x;
			   swapped = true;
		   }
		}
		if(!swapped) break;

	}


}


void Solution::assignProcessInitial(Process* p, Machine* m)
{

	nmb_processes_from_service_on_machine_[p->getService()->getId()][m->getId()] ++;

	int64 oldMachineLoadCost = m->getLoadCost();
	int64 oldMachineBalanceCost = m->getBalanceCost();

	if(m->getNumberOfProcesses() == 0)
		oldMachineBalanceCost = 0;


	p->assignInitial(m);

	load_cost_    =    load_cost_ + (m->getLoadCost()    - oldMachineLoadCost);
	balance_cost_ = balance_cost_ + (m->getBalanceCost() - oldMachineBalanceCost);


}

void Solution::reassignProcess(Process* p, Machine* m){


    if(p->getMachine() == m) return;

	//update check variables
	if(m != 0) {
		nmb_processes_from_service_on_machine_[p->getService()->getId()][m->getId()] ++;
		if(nmb_processes_from_service_on_machine_[p->getService()->getId()][m->getId()] == 2)
			nmb_of_conflicted_machines_++;
	}

	if(p->getMachine() != 0) {
	    nmb_processes_from_service_on_machine_[p->getService()->getId()][p->getMachineId()] --;
	    if(nmb_processes_from_service_on_machine_[p->getService()->getId()][p->getMachineId()] == 1)
	    	nmb_of_conflicted_machines_--;
	}


	int number_of_moved_processes_before = p->getService()->getNumberOfMovedProcesses();
	bool checkSpreadBefore = p->getService()->checkSpread();
	bool checkCapacityOnMachine1Before, checkCapacityOnMachine2Before;

	Machine* oldMachine = p->getMachine();
	if(p->getMachine() != 0)  checkCapacityOnMachine1Before = !p->getMachine()->hasNegativeRemainingCapacity();
    if(m != 0)                checkCapacityOnMachine2Before = !m->hasNegativeRemainingCapacity();



    //update cost variables


	int64 oldMachine1LoadCost = 0;
	int64 oldMachine1BalanceCost = 0;

	if(p->getMachine() != 0)
	{
		oldMachine1LoadCost = p->getMachine()->getLoadCost();
		oldMachine1BalanceCost = p->getMachine()->getBalanceCost();
	}

	int64 oldMachine2LoadCost = 0;
	int64 oldMachine2BalanceCost = 0;

	if(m != 0)
	{
		oldMachine2LoadCost = m->getLoadCost();
		oldMachine2BalanceCost = m->getBalanceCost();
	}


	//*****************************
      p->assign(m);
    //*****************************

	if(!p->getService()->checkSpread() &&  checkSpreadBefore) nmb_of_services_with_small_spread_++;
	if(p->getService()->checkSpread()  && !checkSpreadBefore) nmb_of_services_with_small_spread_--;

	if(oldMachine != 0 && !checkCapacityOnMachine1Before && !oldMachine->hasNegativeRemainingCapacity())
		nmb_of_machines_with_violated_capacity_--;
	if(m != 0 && checkCapacityOnMachine2Before && m->hasNegativeRemainingCapacity())
		nmb_of_machines_with_violated_capacity_++;


	if(m != 0)
	{
		load_cost_    =    load_cost_ + (m->getLoadCost()    - oldMachine2LoadCost);
		balance_cost_ = balance_cost_ + (m->getBalanceCost() - oldMachine2BalanceCost);
	}

	if(oldMachine != 0)
	{
		load_cost_    =    load_cost_ + (oldMachine->getLoadCost()    - oldMachine1LoadCost);
		balance_cost_ = balance_cost_ + (oldMachine->getBalanceCost() - oldMachine1BalanceCost);
	}


	if(p->getInitialMachine() != m && p->getInitialMachine() == oldMachine)
		process_move_cost_ += data_->getProcessMoveCostWeight();

	if(p->getInitialMachine() == m)
		process_move_cost_ -= data_->getProcessMoveCostWeight();

	machine_move_cost_ = machine_move_cost_ + data_->getMachineMoveCostWeight() *
	  (p->getInitialMachine()->getToMachineMoveCost(m) - p->getInitialMachine()->getToMachineMoveCost(oldMachine));


	//data_->calculateProcessesPositionsByRemovingValue();

    //***********************************************************************
    // update number of max number of processes
	// (used for calculating service move cost difference)

	int number_of_moved_processes_after =
			p->getService()->getNumberOfMovedProcesses();
	if(number_of_moved_processes_after > max_number_of_moved_processes_)
	{
		max_number_of_moved_processes_ =
				p->getService()->getNumberOfMovedProcesses();
		number_of_services_with_max_number_of_moved_processes_ = 1;
	}
	else
    if(number_of_moved_processes_after == max_number_of_moved_processes_ &&
       number_of_moved_processes_after > number_of_moved_processes_before)
    {
	  number_of_services_with_max_number_of_moved_processes_ ++;
    }
    else
	if(number_of_moved_processes_after < max_number_of_moved_processes_ &&
	   number_of_moved_processes_before == max_number_of_moved_processes_)
	{
	  if(number_of_services_with_max_number_of_moved_processes_ > 1)
		  number_of_services_with_max_number_of_moved_processes_--;
	  else
	  {
		  // calculate number of moved processes for each service again
		 max_number_of_moved_processes_= 0;
		 number_of_services_with_max_number_of_moved_processes_ = 0;
		 for(vector<Service*>::iterator it = services_.begin();
				 it != services_.end(); it++)
		   if((*it)->getNumberOfMovedProcesses() > max_number_of_moved_processes_)
		   {
			 max_number_of_moved_processes_ = (*it)->getNumberOfMovedProcesses();
			 number_of_services_with_max_number_of_moved_processes_ = 1;
		   }
		   else if((*it)->getNumberOfMovedProcesses() ==
				   max_number_of_moved_processes_)
			   number_of_services_with_max_number_of_moved_processes_++;
	  }
	}
    //*************************************************************************


}

void Solution::calculateLoadCost()
{
	 load_cost_ = 0;
	 for(vector<Machine*>::iterator it = machines_.begin();it != machines_.end(); it++)
	     load_cost_ += (*it)->getLoadCost();
}


int64  Solution::getLoadCost()
{
	  return load_cost_;
}

int64  Solution::getloadCostForResource(int r)
{
  int64 load_cost = 0;
  for(vector<Machine*>::iterator it = machines_.begin();it != machines_.end(); it++) {
     load_cost += (*it)->getLoadCostForResource(r);
  }

  return load_cost;
}

int64  Solution::getBalanceCost()
{
	  return balance_cost_;
}

int64  Solution::getProcessMoveCost()
{
	return process_move_cost_;
}

int64  Solution::getServiceMoveCost()
{
	return max_number_of_moved_processes_ *  data_->getServiceMoveCostWeight();
}

int64  Solution::getMachineMoveCost()
{
	return machine_move_cost_;
}

int64  Solution::getCost()
{

	 return getLoadCost() +
			getBalanceCost() +
			getProcessMoveCost() +
			getMachineMoveCost() +
			getServiceMoveCost() ;
}

bool Solution::check(bool shouldPrint)
{
	  return
	         checkCapacity  (shouldPrint)
	      && checkConflict  (shouldPrint)
	      && checkSpread    (shouldPrint)
	      && checkDependency(shouldPrint)  ;
}



bool Solution::checkCapacity(bool shouldPrint)
{
	return nmb_of_machines_with_violated_capacity_ == 0;
}


bool Solution::checkConflict(bool shouldPrint)
{

  return nmb_of_conflicted_machines_ == 0;
}


bool Solution::checkSpread(bool shouldPrint)
{
	  return nmb_of_services_with_small_spread_ == 0;
}


bool Solution::checkDependency(bool shouldPrint)
{
	  for(vector<Service*>::iterator it = services_.begin(); it != services_.end(); it++)
		  if(!(*it)->checkDependencies())
			  return false;

	  return true;
}



bool Solution::checkCapacityWithShift(Process* p, Machine* m)
{
	return m->checkCapacityWithAddingProcess(p, process_requirements_sorted_[p->getId()]);
}


bool Solution::checkConflictWithShift(Process* p, Machine* m)
{
	return nmb_processes_from_service_on_machine_[p->getService()->getId()][m->getId()] == 0;
}

bool Solution::checkSpreadWithShift(Process* p, Machine* m) {

	return p->getService()->checkSpreadWithShift(p,m);

}


bool Solution::checkDependencyWithShift(Process* p, Machine* m) {

	return p->getService()->checkDependencyWithShift(p,m);

}

bool Solution::checkDependencyWithShiftInHood1(Process* p, Machine* m) {

	return p->getService()->checkDependencyWithShiftInHood1(p,m);

}


bool Solution::checkDependencyWithShiftInHood2(Process* p, Machine* m) {

	return p->getService()->checkDependencyWithShiftInHood2(p,m);

}


bool Solution::checkShift(Process* p, Machine* m) {


	return  checkCapacityWithShift(p, m)   &&
			checkConflictWithShift(p, m)  					    &&
			checkSpreadWithShift(p, m)     						&&
			checkDependencyWithShift(p, m);

}


bool Solution::checkShiftWithNoCapacity(Process* p, Machine* m) {


	return  checkConflictWithShift(p, m)  					    &&
			checkSpreadWithShift(p, m)     						&&
			checkDependencyWithShift(p, m);

}


bool Solution::checkShift(int p, int m) {

	return checkShift(processes_[p], machines_[m]);

}


bool Solution::checkShiftWithNoCapacity(int p, int m) {

	return checkShiftWithNoCapacity(processes_[p], machines_[m]);

}


bool Solution::checkCapacityWithSwap(Process* p1, Process* p2)
{


	if(p2->getPosition() < p1->getPosition())
	  return p1->getMachine()->checkCapacityWithSwap(p1, p2) &&
	         p2->getMachine()->checkCapacityWithSwap(p2, p1);

	return p2->getMachine()->checkCapacityWithSwap(p2, p1) &&
	       p1->getMachine()->checkCapacityWithSwap(p1, p2) ;

}


bool Solution::checkConflictWithSwap(Process* p1, Process* p2)
{

    if(p1->getService()->getId() == p2->getService()->getId())
    	return true;

    return checkConflictWithShift(p1, p2->getMachine()) &&
    	   checkConflictWithShift(p2, p1->getMachine());

}

bool Solution::checkSpreadWithSwap(Process* p1, Process* p2)
{

	if(p1->getService()->getId() == p2->getService()->getId())
	    	return true;

    return  checkSpreadWithShift(p1, p2->getMachine()) &&
    		checkSpreadWithShift(p2, p1->getMachine());

}

bool Solution::checkDependencyWithSwap(Process* p1, Process* p2)
{
	return p1->getService()->checkDependencyWithSwap(p1,p2) &&
		   p2->getService()->checkDependencyWithSwap(p2,p1);
}


// check dependencies in nhood p1 belongs to (<-)
bool Solution::checkDependencyWithSwapInHood1(Process* p1, Process* p2)
{
	return p1->getService()->checkDependencyWithSwapInHood1(p1,p2) &&
		   p2->getService()->checkDependencyWithSwapInHood2(p2,p1);
}


// check dependencies in nhood p2 belongs to (<-)
bool Solution::checkDependencyWithSwapInHood2(Process* p1, Process* p2)
{
	return p1->getService()->checkDependencyWithSwapInHood2(p1,p2) &&
		   p2->getService()->checkDependencyWithSwapInHood1(p2,p1);
}


bool Solution::checkSwap(Process* p1, Process* p2)
{

	return
			checkCapacityWithSwap(p1,p2)  &&
			checkConflictWithSwap(p1,p2)     &&
			checkSpreadWithSwap(p1,p2)       &&
			checkDependencyWithSwap(p1,p2) ;

}

bool Solution::checkSwap(int p1, int p2)
{

	return checkSwap(processes_[p1], processes_[p2]);

}



bool Solution::checkSwapWithNoCapacity(Process* p1, Process* p2)
{

			return
			checkConflictWithSwap(p1,p2)     &&
			checkSpreadWithSwap(p1,p2)       &&
			checkDependencyWithSwap(p1,p2) ;

}

bool Solution::checkSwapWithNoCapacity(int p1, int p2)
{

	return checkSwapWithNoCapacity(processes_[p1], processes_[p2]);

}


int64 Solution::getLoadCostDifferenceWithShift(Process* p, Machine* m)
{

  if(p->getMachine() == 0)
	  return m->getLoadCostDifferenceWithAddingProcess(p);

  return
    p->getMachine()->getLoadCostDifferenceWithRemovingProcess(p) +
                  m->getLoadCostDifferenceWithAddingProcess(p);

}


int64 Solution::getBalanceCostDifferenceWithShift(Process* p, Machine* m)
{

	if(p->getMachine() == 0)
		  return m->getBalanceCostDifferenceWithAddingProcess(p);

    return p->getMachine()->getBalanceCostDifferenceWithRemovingProcess(p) +
                         m->getBalanceCostDifferenceWithAddingProcess(p);
}


int64 Solution::getPMCDifferenceWithShift(Process* p, Machine* m)
{
	if (p->getMachine() == p->getInitialMachine() || m == 0)
		return p->getMoveCost() * data_->getProcessMoveCostWeight();
	if (m == p->getInitialMachine())
		return (-1) * p->getMoveCost() * data_->getProcessMoveCostWeight();

	return 0;

}


int64 Solution::getSMCDifferenceWithShift(Process* p, Machine* m)
{

    if(p->getService()->getNumberOfMovedProcesses() <
    		max_number_of_moved_processes_) return 0;

    if(p->getInitialMachine() == p->getMachine())
    	return data_->getServiceMoveCostWeight();

    if(p->getInitialMachine() == m &&
    		number_of_services_with_max_number_of_moved_processes_ == 1)
     	return (-1)*(data_->getServiceMoveCostWeight());


    return 0;
}


int64 Solution::getMMCDifferenceWithShift(Process* p, Machine* m)
{
	return  (
			p->getInitialMachine()->getToMachineMoveCost(m) -
		    p->getInitialMachine()->getToMachineMoveCost(p->getMachine())
		    ) * data_->getMachineMoveCostWeight();
}



int64 Solution::getCostDifferenceWithShift(Process* p, Machine* m)
{
	return     getLoadCostDifferenceWithShift(p,m)
			+  getBalanceCostDifferenceWithShift(p,m)
			+  getPMCDifferenceWithShift(p,m)
			+  getSMCDifferenceWithShift(p,m)
			+  getMMCDifferenceWithShift(p,m);
}

int64 Solution::getCostDifferenceWithShift(int p, int m)
{
	return  getCostDifferenceWithShift(processes_[p], machines_[m]);
}


int64 Solution::getLoadCostDifferenceWithSwap(Process* p1, Process* p2)
{

	int64 diff = 0;
	if(p1->getMachine() != 0) diff += p1->getMachine()->getLoadCostDifferenceWithSwap(p1,p2);
	if(p2->getMachine() != 0) diff += p2->getMachine()->getLoadCostDifferenceWithSwap(p2,p1);
	return diff;
}


int64 Solution::getBalanceCostDifferenceWithSwap(Process* p1, Process* p2)
{
	int64 diff = 0;
	if(p1->getMachine() != 0) diff += p1->getMachine()->getBalanceCostDifferenceWithSwap(p1,p2);
	if(p2->getMachine() != 0) diff += p2->getMachine()->getBalanceCostDifferenceWithSwap(p2,p1);
	return diff;
}

int64 Solution::getPMCDifferenceWithSwap(Process* p1, Process* p2)
{
  return
	getPMCDifferenceWithShift(p1, p2->getMachine()) +
	getPMCDifferenceWithShift(p2, p1->getMachine());
}

int64 Solution::getSMCDifferenceWithSwap(Process* p1, Process* p2)
{




	if(p1->getService() != p2->getService())
	{

		if(p1->getService()->getNumberOfMovedProcesses() < max_number_of_moved_processes_ &&
		   p2->getService()->getNumberOfMovedProcesses() < max_number_of_moved_processes_ )   return 0;


		if( (p1->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p1->getInitialMachine() == p1->getMachine()) ||
		    (p2->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p2->getInitialMachine() == p2->getMachine())
		  )
			  return data_->getServiceMoveCostWeight();

		if(
			((p1->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p1->getInitialMachine() == p2->getMachine()) ||
		    (p2->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p2->getInitialMachine() == p1->getMachine())) &&
		    number_of_services_with_max_number_of_moved_processes_ == 2
		  )
			  return -data_->getServiceMoveCostWeight();

		if( p1->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p1->getInitialMachine() == p2->getMachine() &&
		    number_of_services_with_max_number_of_moved_processes_ == 1
		   )
			  return -data_->getServiceMoveCostWeight();

		if( p2->getService()->getNumberOfMovedProcesses() == max_number_of_moved_processes_ &&  p2->getInitialMachine() == p1->getMachine() &&
		    number_of_services_with_max_number_of_moved_processes_ == 1
		   )
			  return -data_->getServiceMoveCostWeight();


        return 0;
	}


	int old_nmb_moved = p1->getService()->getNumberOfMovedProcesses();

	if(old_nmb_moved < max_number_of_moved_processes_ - 1) return 0;

	int new_nmb_moved = old_nmb_moved;
	if(p1->getInitialMachine() == p1->getMachine()) new_nmb_moved ++;
	if(p2->getInitialMachine() == p2->getMachine()) new_nmb_moved ++;
	if(p1->getInitialMachine() == p2->getMachine()) new_nmb_moved --;
	if(p2->getInitialMachine() == p1->getMachine()) new_nmb_moved --;

	if(new_nmb_moved > max_number_of_moved_processes_)
	{
		return (new_nmb_moved - max_number_of_moved_processes_) * data_->getServiceMoveCostWeight();
	}

	if(new_nmb_moved == max_number_of_moved_processes_) return 0;

	if(old_nmb_moved == max_number_of_moved_processes_ &&
	   number_of_services_with_max_number_of_moved_processes_ == 1) return  -data_->getServiceMoveCostWeight();



	return 0;


}

int64 Solution::getMMCDifferenceWithSwap(Process* p1, Process* p2)
{
	  return
		getMMCDifferenceWithShift(p1, p2->getMachine()) +
		getMMCDifferenceWithShift(p2, p1->getMachine());
}


int64 Solution::getCostDifferenceWithSwap(Process* p1, Process* p2)
{

	return
		(	getLoadCostDifferenceWithSwap(p1,p2) +
			getBalanceCostDifferenceWithSwap(p1,p2) +
			getPMCDifferenceWithSwap(p1,p2) +
			getSMCDifferenceWithSwap(p1,p2) +
			getMMCDifferenceWithSwap(p1,p2)
	    );
}

int64 Solution::getCostDifferenceWithSwap(int p1, int p2)
{
	return  getCostDifferenceWithSwap(processes_[p1], processes_[p2]);
}


void Solution::writeToFile(string file_name)
{
	fstream fileOut(file_name.c_str(), ios_base::out);
	for(vector<Process*>::iterator it = processes_.begin(); it != processes_.end(); it++)
	  fileOut << (*it)->getMachine()->getId() << "  ";
}


int Solution::getNumberOfResources()
{
	return data_->getNumberOfResources();
}

Data* Solution::getData()
{
	return data_;
}

int Solution::getNumberOfNonassignedProcesses()
{
	int nmb = 0;
	for(vector<Process*>::iterator it = processes_.begin();
			it != processes_.end(); it++)
		if((*it)->getMachine() == 0) nmb++;
	return nmb;
}


int64 Solution::getLoadCostLB()
{
	return data_->getLoadCostLB();
}


void Solution::setAssignments(vector<int> assignments)
{
	for(int i = 0; i < processes_.size(); i++)
	  reassignProcess(i, assignments[i]);
}

vector<int> Solution::getAssignmentsVectorCopy()
{
	 vector<int> assignments_copy(processes_.size(), -1);
	 for(int i = 0; i < processes_.size(); i++)
	 {
		 if(processes_[i]->getMachine() != 0)
			  assignments_copy[i] = processes_[i]->getMachine()->getId();
			else
			  assignments_copy[i] = -1;
	 }

	 return assignments_copy;
}



int Solution::getNumberOfMachinesProcessCanBeAssignedTo(int p)
{

   int number = 0;
   for(int i = 0; i < machines_.size(); i++)
	   if(matrix_[p][i] == true) number++;
   return number;

}

int Solution::getNumberOfProcessesThatCanBeAssignedToOnlyOneMachine()
{

   int number = 0;
   for(int p = 0; p < processes_.size(); p++)
	   if(getNumberOfMachinesProcessCanBeAssignedTo(p) == 1) number++;
   return number;

}

void Solution::fillShiftMatrix()
{


	int nmbMachines = data_->getNumberOfMachines();
	int nmbProcesses = data_->getNumberOfProcesses();

	for(int m = 0; m < nmbMachines; m++)
	 for(int p = 0; p < nmbProcesses; p++) {
		if(!matrix_[p][m]) shift_matrix_[m][p] = false;
		else shift_matrix_[m][p] = checkCapacityWithShift(processes_[p], machines_[m]);

		if(!matrix_[p][m]) cost_difference_matrix_[m][p] = 1e15;
		else cost_difference_matrix_[m][p] = machines_[m]->getCostDifferenceWithAddingProcess(processes_[p]);
	 }

}


bool abs_position(Process* p1, Process* p2)
{
	return p1->getPosition() < p2->getPosition();
}


void Solution::sortProcessesByPosition()
{
	processes_sorted = processes_;
	sort(processes_sorted.begin(), processes_sorted.end(), abs_position);
}

