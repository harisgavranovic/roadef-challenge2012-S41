#ifndef SERVICE_H_
#define SERVICE_H_

#include <vector>
using namespace std;

#include "process.h"
#include "location.h"
#include "neighborhood.h"
#include "data.h"

class Data;
class Location;
class Neighborhood;

class Service {

 private:

	int id_;
    int spread_min_;

    // current spread
    int spread_;

    // number of processes moved from initial machine in current solution
    int number_of_moved_processes_;

    vector<Process*> processes_;

    // services that depend on this service
    vector<Service*> services_dependendent_;

    // services that this service depends from
    vector<Service*> services_dependends_;

    vector<bool> depends_on_;

    vector<int> number_of_processes_in_location_;
    vector<int> number_of_processes_in_neighborhood_;

    vector<Location*>      locations_;
    vector<Neighborhood*>  neighborhoods_;

    Data* data_;

    void addDependencyReversed(Service* s)    {
    	services_dependendent_.push_back(s);
    }


 public:

    Service(int id, int spread_min, Data* data);

    void setSpreadMin(int s_min);

    void InitializeVectors(Data* data);

    void addProcess(Process* process) {  processes_.push_back(process);       }

    void addDependency(Service* s)    {

    	services_dependends_.push_back(s);
    	s->addDependencyReversed(this);
    	depends_on_[s->getId()] = true;

    }

    void addDependendenttService(Service* s)    {  services_dependendent_.push_back(s); }


    void addLocation(Location* loc);
    void removeLocation(Location* loc);
    void addNeighborhood(Neighborhood* hood);
    void removeNeighborhood(Neighborhood* hood);

    int getId()        { return id_;         }
    int getSpreadMin() { return spread_min_; }

    int getNumberOfProcesses() {return processes_.size(); }
    int getNumberOfServicesDependendentFrom() { return services_dependendent_.size() ; }
    int getNumberOfServicesDependendsOn()     { return services_dependends_.size() ;     }

    int getNumberOfLocations() {return locations_.size();}
    int getNumberOfNeighborhoods() {return neighborhoods_.size();}

    vector<Process*> & getProcesses()                {return processes_;                 }
    vector<Service*> & getServicesDependendentFrom() {return services_dependendent_; }
    vector<Service*> & getServicesDependendsOn()     {return services_dependends_;     }

    Process* getProcess(int i)                 { return processes_.at(i);                }
    Service* getServiceDependendentFrom(int i) {return services_dependendent_.at(i); }
    Service* getServiceDependendsOn(int i)     {return services_dependends_.at(i);     }

    bool dependsOn(Service* s) {return depends_on_[s->getId()];}

    int  getSpread()   {return spread_;}

    bool checkSpread() {return spread_ >= spread_min_;}
    bool checkDependencies();
    int getNumberOfViolatedDependencies();

    Location*     getLocation(int i)     {return locations_.at(i);     }
    Neighborhood* getNeighborhood(int i) {return neighborhoods_.at(i); }

    int getNumberOfProcessesInLocation(int location_id) {return number_of_processes_in_location_.at(location_id);}
    int getNumberOfProcessesInNeighborhood(int hood_id) {return number_of_processes_in_neighborhood_.at(hood_id);}

    int getNumberOfProcessesInLocation(Location* loc) ;
    int getNumberOfProcessesInNeighborhood(Neighborhood* hood) ;

    // update service data when process moved from m0 to m
    void updateWithProcessReassign(Process* p, Machine* m0, Machine* m);


    int64 getNumberOfMovedProcesses() { return number_of_moved_processes_; }

    bool hasProcessOnMachine(Machine* m);
    bool checkSpreadWithShift(Process* p, Machine* m);

    bool checkDependencyWithShift(Process* p, Machine* m);

    bool checkDependencyWithShiftInHood1(Process* p, Machine* m);
    bool checkDependencyWithShiftInHood2(Process* p, Machine* m);


    bool checkDependencyWithSwap(Process* p1, Process* p2);

    bool checkDependencyWithSwapInHood1(Process* p1, Process* p2);
    bool checkDependencyWithSwapInHood2(Process* p1, Process* p2);
};



#endif /* SERVICE_H_ */
