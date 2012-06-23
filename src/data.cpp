#include "data.h"


void Data::addLocation(Location* l)

{

   if(locations_.size() > l->getId() && locations_.at(l->getId()) == 0)
        locations_.at(l->getId()) = l;
   else if(l->getId() == locations_.size())
	  locations_.push_back(l);
   else {
	   int diff = l->getId() - locations_.size();
	   for(int i = 0; i < diff; i++) locations_.push_back(0);
	   locations_.push_back(l);
   }

}


void Data::addNeighborhood(Neighborhood* n)
{

   if(neighborhoods_.size() > n->getId() && neighborhoods_.at(n->getId()) == 0)
	   neighborhoods_.at(n->getId()) = n;
   else if(n->getId() == neighborhoods_.size())
	   neighborhoods_.push_back(n);
   else {
	   int diff = n->getId() - neighborhoods_.size();
	   for(int i = 0; i < diff; i++)
		   neighborhoods_.push_back(0);
	   neighborhoods_.push_back(n);
   }

}

// calculate load cost lower bound
// lower bound is equal to (S1 - S2) * load_cost_weight, where
// S1 = and sum all process requirements
// S2 = sum of safety capacities over all machines

int64 Data::getLoadCostLB(int resource_)
{

   vector<int64> total_requirements(resources_.size(),0);
   vector<int64> total_safety_capacity(resources_.size(),0);


   for(int r = 0; r < resources_.size(); r++)
   {
	   for(int p = 0; p < processes_.size(); p++)
		   total_requirements[r] += processes_[p]->getRequirement(r);

	   for(int m = 0; m < machines_.size(); m++) {
		   total_safety_capacity[r] += machines_[m]->getMachineResource(r)->getSafetyCapacity();

	   }
   }

   int64 lower_bound = 0;

   // only one resource
   if(resource_ >= 0)
     return max( (total_requirements[resource_] - total_safety_capacity[resource_]) *
    		        resources_[resource_]->load_cost_weight, 0LL) ;

   for(int r = 0; r < resources_.size(); r++)
   {
	   lower_bound += max(
			   (total_requirements[r] - total_safety_capacity[r]) * resources_[r]->load_cost_weight, 0LL) ;
   }

   return lower_bound;

}



bool greaterProcessAbs(Process* p1, Process* p2)
{

   return p1->getTotalRequirement() > p2->getTotalRequirement();

}



void Data::calculateProcessesPositionsByRequirements()
{

   vector<Process*> processes_copy = processes_;
   sort(processes_copy.begin(), processes_copy.end(), greaterProcessAbs);

   for(vector<Process*>::iterator it = processes_copy.begin(); it  != processes_copy.end(); it++)
	   (*it)->setAbsPosition(it - processes_copy.begin());


}
