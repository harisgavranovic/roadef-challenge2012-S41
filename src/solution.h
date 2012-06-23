#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <vector>
using namespace std;


#include "data.h"
#include "parameters.h"

class Solution {

 private:

	// all processes - ordered by id
	vector<Process*> processes_;

	// all machines - ordered by id
	vector<Machine*> machines_;

	// all services - ordered by id
	vector<Service*> services_;

	// all processes - ordered by position
	vector<Process*> processes_sorted;

    // max number of moved processes from the same service
    int max_number_of_moved_processes_;
    int number_of_services_with_max_number_of_moved_processes_;

    Data* data_;

 private:

    // check variables
    int** nmb_processes_from_service_on_machine_;
    int   nmb_of_conflicted_machines_;
    int   nmb_of_machines_with_violated_capacity_;
    int** nmb_of_services_with_small_spread_;

    // cost variables
    int64   load_cost_;
    int64   balance_cost_;
    int64   process_move_cost_;
    int64   machine_move_cost_;

  public:
     // used when changing load weights
     void calculateLoadCost();

 public:

    Parameters params_;
    void setParams(Parameters& params) {params_ = params;}

    bool** matrix_;

    bool** shift_matrix_;
    int64**  cost_difference_matrix_;
    void   fillShiftMatrix();


    int** process_requirements_sorted_;

    void sortProcessesByPosition();

    vector<Process*>& getSortedProcesses()        {return processes_sorted;}
    vector<Process*>  getSortedProcessesCopy()    {return processes_sorted;}
    Process*          getProcessAtPosition(int i) {return processes_sorted[i];}


 public:

    Solution();

    void setData(Data* data);

    void addProcess(Process* p);
    void addMachine(Machine* m) { machines_.push_back(m);  }
    void addService(Service* s) { services_.push_back(s);  }


    Process* getProcess(int id) { return processes_.at(id); }
    Machine* getMachine(int id) { return machines_.at(id);  }
    Service* getService(int id) { return services_.at(id);  }


    //******************ASSIGNING PROCESES**********************
    void assignProcessInitial(Process* p, Machine* m);
    void reassignProcess(Process* p, Machine* m);
    void reassignProcess(int p, int m){
    	if(m >= 0) reassignProcess(processes_.at(p), machines_.at(m));
    	else       reassignProcess(processes_.at(p), 0);
    }
    //***********************************************************

    //******************CALCULATING COSTS************************
    int64 getCost();
    int64 getLoadCost();
    int64 getBalanceCost();
    int64 getProcessMoveCost();
    int64 getServiceMoveCost();
    int64 getMachineMoveCost();

    int64 getloadCostForResource(int r);
    int64 getLoadAndBalanceCost();
    //***********************************************************


    //******************CHECKING CONSTRAINTS**********************
    // Checks hard constraints.
    // Returns true if constraints are satisfied, false otherwise.
    bool check(bool shouldPrint = false);
    bool checkCapacity(bool shouldPrint = false);
    bool checkConflict(bool shouldPrint = false);
    bool checkSpread(bool shouldPrint = false);
    bool checkDependency( bool shouldPrint = false);
    //***********************************************************


    //*********CHECK CONSTRAINTS WITH SHIFT*****************************************
    bool checkCapacityWithShift(Process* p, Machine* m);
    bool checkConflictWithShift(Process* p, Machine* m);
    bool checkSpreadWithShift(Process* p, Machine* m);
    bool checkDependencyWithShift(Process* p, Machine* m);

    bool checkShift(Process* p, Machine* m);
    bool checkShift(int p, int m);

    bool checkShiftWithNoCapacity(Process* p, Machine* m);
    bool checkShiftWithNoCapacity(int p, int m);

    bool checkDependencyWithShiftInHood1(Process* p, Machine* m);
    bool checkDependencyWithShiftInHood2(Process* p, Machine* m);

    //*******************************************************************************


    //*********CHECK CONSTRAINTS WITH SWAP*******************************************
    bool checkCapacityWithSwap(Process* p1, Process* p2);
    bool checkConflictWithSwap(Process* p1, Process* p2);
    bool checkSpreadWithSwap(Process* p1, Process* p2);
    bool checkDependencyWithSwap(Process* p1, Process* p2);

    bool checkSwap(Process* p1, Process* p2);
    bool checkSwap(int p1, int p2);

    bool checkSwapWithNoCapacity(Process* p1, Process* p2);
    bool checkSwapWithNoCapacity(int p1, int p2);

    bool checkDependencyWithSwapInHood1(Process* p1, Process* p2);
    bool checkDependencyWithSwapInHood2(Process* p1, Process* p2);
    //********************************************************************************



    //*********CALCULATING COST DIFFERENCES WITH SHIFT*******************
    int64 getLoadCostDifferenceWithShift(Process* p, Machine* m);
    int64 getBalanceCostDifferenceWithShift(Process* p, Machine* m);
    int64 getPMCDifferenceWithShift(Process* p, Machine* m);
    int64 getSMCDifferenceWithShift(Process* p, Machine* m);
    int64 getMMCDifferenceWithShift(Process* p, Machine* m);
    int64 getCostDifferenceWithShift(Process* p, Machine* m);
    int64 getCostDifferenceWithShift(int p, int m);
    //*******************************************************************

    //*********CALCULATING COST DIFFERENCES WITH SWAP********************
    int64 getLoadCostDifferenceWithSwap(Process* p1, Process* p2);
    int64 getBalanceCostDifferenceWithSwap(Process* p1, Process* p2);
    int64 getPMCDifferenceWithSwap(Process* p1, Process* p2);
    int64 getSMCDifferenceWithSwap(Process* p1, Process* p2);
    int64 getMMCDifferenceWithSwap(Process* p1, Process* p2);
    int64 getCostDifferenceWithSwap(Process* p1, Process* p2);
    int64 getCostDifferenceWithSwap(int p1, int p2);
    //*******************************************************************


    //***********PRINTING*******************
    bool printCheck() ;
    void printCosts();
    void writeToFile(string file_name);
    void printStats(string fileOut, string dataFileName);
    //**************************************


    int getNumberOfProcesses() {return processes_.size();}
    int getNumberOfMachines () {return machines_.size();}
    int getNumberOfServices () {return services_.size();}
    int getNumberOfResources() ;

    // fill the matrix matrix_
    // matrix[p][m] = true if p can be assigned to m when m empty
    void fillMatrix();

    int64 getLoadCostLB();

    // set new assignments
    // needed when solution neds to be loaded
    void setAssignments(vector<int> assignments);
    vector<int> getAssignmentsVectorCopy();

    Data* getData();

    int getNumberOfNonassignedProcesses();


    //**************************************************************************************
    // za boost
    int64 getLoadBalanceCheckRemainingCapWithMoveP1toM2andRemovingP2(int p1, int p2);
    bool  checkRemainingCapacitiesWithMoveP1InMachineP2(int p1, int p2);
    bool  checkConflictConstraintsWithMoveP1InMachineP2(int p1, int p2);
    int64 getBalanceCostDifferenceWithMoveP1InMachineP2(int p1, int p2);
    int64 getLoadBalanceCheckRemainingCapForGivenChainShift(vector<int>& vv, int plength);
    int64 GetLoadBalanceCheckRemainingCapForGivenCycle(vector<int>& vv, int plength);
    int64 effectuateChainShiftMoveForGivenChain(vector<int>& vec_cycle, int plength);
    //**************************************************************************************

    bool checkPath(vector<int>& processes_in_chain,  int length, string constraints_string = "111") ;
    bool checkCycle(vector<int>& processes_in_cycle, int length, string constraints_string = "111") ;

    vector<Machine*> getMachinesCopy()  {return machines_;  }
    vector<Process*> getProcessesCopy() {return processes_; }

    int getNumberOfMachinesProcessCanBeAssignedTo(int p);
    int getNumberOfProcessesThatCanBeAssignedToOnlyOneMachine();

    int getNumberOfMachinesWithPositiveSafetyRemCapacity();

    double getTotalOvercapacity();

    void setOriginalLoadCostWeights()
    {
	   for(int r = 0; r < getNumberOfResources(); r++)
			  data_->getResource(r)->load_cost_weight = data_->getResource(r)->original_load_cost_weight;
	   calculateLoadCost();
    }


    void changeLoadCostWeights(int rr, int weight_decrease_factor)
    {
	   for(int r = 0; r < getNumberOfResources(); r++)
		if(r != rr /*&& r != rr + 1*/)
	   		 data_->getResource(r)->load_cost_weight = data_->getResource(r)->load_cost_weight / weight_decrease_factor;
	   calculateLoadCost();
    }


    string solutionFilename;
    int64 best_cost_;

    void updateBestCost() {

    	int64 currentCost = getCost();
    	if(best_cost_ <= currentCost) return;
        best_cost_ = currentCost;
   	    writeToFile(solutionFilename);

   	    if(best_cost_ < *(params_.best_objective))
   	    {
   	   	    *(params_.best_objective) = best_cost_;
   	    	writeToFile(params_.solution_filename);
   	    }
    }

    void setSolutionFilename(string file_name)
    {
    	solutionFilename = file_name;
    }



};



#endif /* SOLUTION_H_ */
