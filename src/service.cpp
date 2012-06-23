#include "service.h"



Service::Service(int id, int spread_min, Data* data)
    : id_(id), spread_min_(spread_min),
      number_of_moved_processes_(0), spread_(0), data_(data)
{

	processes_.resize(0);
	services_dependendent_.resize(0);
	services_dependends_.resize(0);
	locations_.resize(0);
	neighborhoods_.resize(0);
	number_of_processes_in_location_.resize(0);
	number_of_processes_in_neighborhood_.resize(0);

}


void Service::setSpreadMin(int s_min) {spread_min_ = s_min;}

void Service::InitializeVectors(Data* data)
{

	data_ = data;
    depends_on_.resize(data->getNumberOfServices(), false);
    number_of_processes_in_location_.resize(data->getNumberOfLocations(), 0);
    number_of_processes_in_neighborhood_.resize(data->getNumberOfNeighborhoods(), 0);

}

int Service::getNumberOfProcessesInLocation(Location* loc)
{
	return number_of_processes_in_location_.at(loc->getId());
}

void Service::addLocation(Location* loc)
{

	// add location if it doesn't exist
	if(number_of_processes_in_location_[loc->getId()] < 1) {
		locations_.push_back(loc);
		spread_ ++;
	}
}

void Service::removeLocation(Location* loc)
{
	// remove location if only one process from service runs on it
	if(number_of_processes_in_location_[loc->getId()] < 2) {
		vector<Location*>::iterator it;
		for (it = locations_.begin(); (*it) != loc; it++);
		locations_.erase(it);
		spread_ --;
	}
}

int Service::getNumberOfProcessesInNeighborhood(Neighborhood* hood) {
	return number_of_processes_in_neighborhood_.at(hood->getId());
}


void Service::addNeighborhood(Neighborhood* hood)
{
	if(number_of_processes_in_neighborhood_[hood->getId()] < 1) neighborhoods_.push_back(hood);
}

void Service::removeNeighborhood(Neighborhood* hood)
{
	// remove neighborhood if only one process from service runs on it
	if(number_of_processes_in_neighborhood_[hood->getId()] < 2) {
		vector<Neighborhood*>::iterator it;
		for (it = neighborhoods_.begin(); (*it) != hood; it++);
		neighborhoods_.erase(it);
	}
}


// update service data when process moved from m0 to m
void Service::updateWithProcessReassign(Process* p, Machine* m0, Machine* m)
{

   if(m0 == 0) // process was not assigned
   {
	  if(m != 0) {
		  addLocation(m->getLocation());
		  addNeighborhood(m->getNeighborhood());
		  number_of_processes_in_location_[m->getLocation()->getId()] ++;
		  number_of_processes_in_neighborhood_[m->getNeighborhood()->getId()] ++;
	  }
      if(p->getInitialMachine() ==  m)
    	number_of_moved_processes_ --;
   }
   else
   {

	  if(m == 0)
	  {
		  removeLocation(m0->getLocation());
		  number_of_processes_in_location_[m0->getLocation()->getId()] --;
	  }
	  else
	  if(m->getLocation() != m0->getLocation())
	  {
		  addLocation(m->getLocation());
		  number_of_processes_in_location_[m->getLocation()->getId()] ++;
		  removeLocation(m0->getLocation());
		  number_of_processes_in_location_[m0->getLocation()->getId()] --;
	  }

	  if(m == 0)
	  {
		  removeNeighborhood(m0->getNeighborhood());
		  number_of_processes_in_neighborhood_[m0->getNeighborhood()->getId()] --;
	  }
	  else
	  if(m->getNeighborhood() != m0->getNeighborhood())
	  {
		  addNeighborhood(m->getNeighborhood());
		  number_of_processes_in_neighborhood_[m->getNeighborhood()->getId()] ++;
		  removeNeighborhood(m0->getNeighborhood());
		  number_of_processes_in_neighborhood_[m0->getNeighborhood()->getId()] --;
	  }

	  if(m == 0) {
		  //number_of_moved_processes_ ++;
	  }
	  else
	  if(p->getInitialMachine() == m0 && p->getInitialMachine() !=  m)
		  number_of_moved_processes_ ++;
	  else
	  if(p->getInitialMachine() ==  m && p->getInitialMachine() != m0)
		  number_of_moved_processes_ --;

   }

}



// check dependency constraint
// only check services that current service depends on
bool Service::checkDependencies()
{
   for(int n = 0; n < data_->getNumberOfNeighborhoods(); n++)
   {
      if(number_of_processes_in_neighborhood_[n] < 1) continue;
      for(vector<Service*>::const_iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
    	  if((*it)->getNumberOfProcessesInNeighborhood(n) < 1)
    		  return false;
   }
   return true;
}


int Service::getNumberOfViolatedDependencies()
{
   int nmb = 0;
   for(vector<Service*>::iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
   {
	   for(int n = 0; n < data_->getNumberOfNeighborhoods(); n++)
       {
          if(number_of_processes_in_neighborhood_[n] < 1) continue;
    	  if((*it)->getNumberOfProcessesInNeighborhood(n) < 1) {
    		  nmb++;
    		  break;
    	  }
       }
   }
   return nmb;
}


bool Service::hasProcessOnMachine(Machine* m)
{
   for(vector<Process*>::iterator it = processes_.begin(); it != processes_.end(); it++)
	  if((*it)->getMachine() == m) return true;
   return false;
}


bool Service::checkSpreadWithShift(Process* p, Machine* m) {

	if(spread_ > spread_min_) return true;
	if(p->getMachine()->getLocation() == m->getLocation()) return true;

	int l1 = p->getMachine()->getLocation()->getId();
	int l2 =               m->getLocation()->getId();

	return number_of_processes_in_location_[l1] > 1 || (number_of_processes_in_location_[l2] == 0) ;

}



bool Service::checkDependencyWithShift(Process* p, Machine* m)
{


	//return checkDependencyWithShiftInHood1(p, m) && checkDependencyWithShiftInHood2(p, m);

	if( p->getMachine()->getNeighborhood() ==  m->getNeighborhood()) return true;

	// check ->

	int n1 = p->getMachine()->getNeighborhood()->getId();

    // p is the only process from service in n1
    if(number_of_processes_in_neighborhood_[n1] == 1)
    {
	    for(vector<Service*>::iterator it = services_dependendent_.begin(); it != services_dependendent_.end(); it++)
	      if((*it)->getNumberOfProcessesInNeighborhood(n1) > 0) return false;
    }


    // check <-

  	int n2 = m->getNeighborhood()->getId();

    // there is no processes from service in n2
    if(number_of_processes_in_neighborhood_[n2] == 0)
    {
  	    for(vector<Service*>::iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
  	      if((*it)->getNumberOfProcessesInNeighborhood(n2) == 0) return false;
    }

    return true;

}



// check dependencies in nhood p belongs to (->)
bool Service::checkDependencyWithShiftInHood1(Process* p, Machine* m)
{

	if( p->getMachine()->getNeighborhood() ==  m->getNeighborhood()) return true;

	int n1 = p->getMachine()->getNeighborhood()->getId();

    // p is the only process from service in n1
    if(number_of_processes_in_neighborhood_[n1] == 1)
    {
	    for(vector<Service*>::iterator it = services_dependendent_.begin(); it != services_dependendent_.end(); it++)
	      if((*it)->getNumberOfProcessesInNeighborhood(n1) > 0) return false;
    }

    return true;

}

// check dependencies in nhood m belongs to (<-)
bool Service::checkDependencyWithShiftInHood2(Process* p, Machine* m)
{

	if( p->getMachine()->getNeighborhood() ==  m->getNeighborhood()) return true;

  	int n2 = m->getNeighborhood()->getId();

    // there is no processes from service in n2
    if(number_of_processes_in_neighborhood_[n2] == 0)
    {
  	    for(vector<Service*>::iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
  	      if((*it)->getNumberOfProcessesInNeighborhood(n2) == 0) return false;
    }

    return true;

}



bool Service::checkDependencyWithSwap(Process* p1, Process* p2)
{


	//return checkDependencyWithSwapInHood1(p1, p2) && checkDependencyWithSwapInHood2(p1,p2);

	if(this == p2->getService() ||
     p1->getMachine()->getNeighborhood() == p2->getMachine()->getNeighborhood())
	   return true;

	// check ->

	int n1 = p1->getMachine()->getNeighborhood()->getId();

	// p1 is the only process from service in n1 and it is removed from n1
	if(number_of_processes_in_neighborhood_[n1] == 1 &&
	   p1->getMachine()->getNeighborhood() != p2->getMachine()->getNeighborhood())
	{
		if(p2->getService()->dependsOn(this)) return false;
		for(vector<Service*>::iterator it = services_dependendent_.begin(); it != services_dependendent_.end(); it++)
			if((*it)->getNumberOfProcessesInNeighborhood(n1) > 0)
				return false;
	}


	// check <-

	int n2 = p2->getMachine()->getNeighborhood()->getId();

	// there is no processes from service in n2
	if(number_of_processes_in_neighborhood_[n2] == 0)
	{
	   if(dependsOn(p2->getService()) &&
		  p2->getService()->getNumberOfProcessesInNeighborhood(n2) == 1)
		   return false;
	   for(vector<Service*>::iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
		  if((*it)->getNumberOfProcessesInNeighborhood(n2) == 0) return false;
	}

	return true;

}


bool Service::checkDependencyWithSwapInHood1(Process* p1, Process* p2)
{

	if(this == p2->getService() ||
     p1->getMachine()->getNeighborhood() == p2->getMachine()->getNeighborhood())
	   return true;

	// check ->

	int n1 = p1->getMachine()->getNeighborhood()->getId();

	// p1 is the only process from service in n1 and it is removed from n1
	if(number_of_processes_in_neighborhood_[n1] == 1 &&
	   p1->getMachine()->getNeighborhood() != p2->getMachine()->getNeighborhood())
	{
		if(p2->getService()->dependsOn(this)) return false;
		for(vector<Service*>::iterator it = services_dependendent_.begin(); it != services_dependendent_.end(); it++)
			if((*it)->getNumberOfProcessesInNeighborhood(n1) > 0)
				return false;
	}

	return true;

}



bool Service::checkDependencyWithSwapInHood2(Process* p1, Process* p2)
{

	if(this == p2->getService() ||
     p1->getMachine()->getNeighborhood() == p2->getMachine()->getNeighborhood())
	   return true;


	// check <-

	int n2 = p2->getMachine()->getNeighborhood()->getId();

	// there is no processes from service in n2
	if(number_of_processes_in_neighborhood_[n2] == 0)
	{
	   if(dependsOn(p2->getService()) &&
		  p2->getService()->getNumberOfProcessesInNeighborhood(n2) == 1)
		   return false;
	   for(vector<Service*>::iterator it = services_dependends_.begin(); it != services_dependends_.end(); it++)
		  if((*it)->getNumberOfProcessesInNeighborhood(n2) == 0) return false;
	}

	return true;

}

