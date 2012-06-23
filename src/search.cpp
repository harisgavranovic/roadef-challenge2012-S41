#include "search.h"

Parameters PARAMS_;


// random process and random machine are chosen
// check and cost difference are calculated
// if move is feasible and good it is done
Solution* local_search_shift_random(Solution* sol, Parameters& params)
{

	unsigned int nmbProcesses( sol->getNumberOfProcesses() );
	unsigned int nmbMachines ( sol->getNumberOfMachines() );
	unsigned int counter(0);

	while( counter++ < params.nmb_of_iterations_in_local_search){

		unsigned int p(rand() % nmbProcesses);
		unsigned int m(rand() % nmbMachines);

		if(sol->getProcess(p)->getMachine() == sol->getMachine(m)) continue;
		if(!sol->matrix_[p][m]) continue;

	     if(sol->checkShift(p, m) &&
		    sol->getCostDifferenceWithShift(sol->getProcess(p), sol->getMachine(m)) < 0)
		 {
			 sol->reassignProcess(sol->getProcess(p), sol->getMachine(m));
		 }
	}

	return sol;

}

// 2 random processes are chosen
// check and cost difference are calculated
// if move is feasible and good it is done
Solution* local_search_swap_random (Solution* sol, Parameters& params)
{


	unsigned int nmbProcesses( sol->getNumberOfProcesses() );
	unsigned int counter(0);
	while( counter++ < params.nmb_of_iterations_in_local_search){

		 unsigned int p1(rand() % nmbProcesses);
		 unsigned int p2(rand() % nmbProcesses);

		 if(sol->getProcess(p1)->getMachine() == sol->getProcess(p2)->getMachine()) continue;
		 int m1 = sol->getProcess(p1)->getMachine()->getId();
		 int m2 = sol->getProcess(p2)->getMachine()->getId();
		 if(!sol->matrix_[p1][m2] || !sol->matrix_[p2][m1]) continue;

		 if(sol->checkSwap(p1, p2)
				 && sol->getCostDifferenceWithSwap(sol->getProcess(p1), sol->getProcess(p2)) < 0)
		 {
			 Machine* mach1 = sol->getProcess(p1)->getMachine();
			 Machine* mach2 = sol->getProcess(p2)->getMachine();
			 sol->reassignProcess(sol->getProcess(p1), mach2);
			 sol->reassignProcess(sol->getProcess(p2), mach1);
		 }
	 }

	return sol;

}



vector<int64> getRandomPair(set<vector<int64> >& pairs, int numberOfBestMovesToChooseFrom = 100000000)
{
	 set<vector<int64> >::iterator it = pairs.begin();

	 if(numberOfBestMovesToChooseFrom > pairs.size())
		 numberOfBestMovesToChooseFrom = pairs.size();

	 int random_nmb = rand() % numberOfBestMovesToChooseFrom;
	 for(int i = 0; i < random_nmb; i++) it++;
	 return *it;
}


bool fillTheSetOfGoodShiftPairsB(Solution* sol, Parameters& params, set<vector<int64>  >& pairs, int start_pos =  0, int end_pos =  50000)
{


	int64 fillStartTime = time(0);
	int64 timeLimitForFill = 15 * params.time_limit / 300;

	int max_number_of_processes_to_try = 10000;
	int max_number_of_machines_to_try = 1000;

	vector<Process*> processesCopy = sol->getSortedProcessesCopy();
    vector<Machine*> machinesCopy = sol->getMachinesCopy();

    if(end_pos > sol->getNumberOfProcesses() - 1)
    	end_pos = sol->getNumberOfProcesses() - 1;

    random_shuffle(processesCopy.begin() + start_pos, processesCopy.begin() + end_pos + 1);
    random_shuffle(machinesCopy.begin(), machinesCopy.end());

	for(int i = start_pos; i <= end_pos && i < sol->getNumberOfProcesses() && i < max_number_of_processes_to_try; i++)
	{

	   if(time(0) - fillStartTime > timeLimitForFill) break;

	   int p = processesCopy[i]->getId();

	   if(sol->getProcess(p)->getMachine() == 0) continue;

	   for(unsigned int j = 0; j < sol->getNumberOfMachines() && j < max_number_of_machines_to_try; j++) {

		   int m = machinesCopy[j]->getId();

		   if(!sol->matrix_[p][m]) continue;

		   if(sol->checkShift(p, m))
		   {
	  		   double diff = sol->getCostDifferenceWithShift(p, m);
			   if(diff < 0)
			   {
				  vector<int64> v(3);
				  v[1] = p, v[2] = m, v[0] = diff;
				  pairs.insert(v);

				   //stop if max_number_of_moves reached(B instances)
				   if(sol->getNumberOfProcesses() > 1000)
				   {
					   int max_number_of_moves = 50000;
					   if(sol->getNumberOfProcesses() < 5001)  max_number_of_moves = 75000;
					   if(pairs.size() > max_number_of_moves) return true;
				   }
			   }
		  }

	   }
 	}

	return true;

}



bool fillTheSetOfGoodSwapPairsB(Solution* sol, Parameters& params, set<vector<int64> >& pairs, int start_pos =  0, int end_pos =  50000)
{

	int64 fillStartTime = time(0);
	int64 timeLimitForFill = 5 * params.time_limit / 300;

	int max_number_of_processes_to_try = 2500;

	vector<Process*> processesCopy2 = sol->getSortedProcesses();

    if(end_pos > sol->getNumberOfProcesses() - 1)
    	end_pos = sol->getNumberOfProcesses() - 1;

    vector<Process*> processesCopy(processesCopy2.begin() + start_pos,processesCopy2.begin() + end_pos - start_pos + 1);
    random_shuffle(processesCopy.begin(), processesCopy.end());

    for(unsigned int i = 0; i < processesCopy.size() && i < max_number_of_processes_to_try; i++){

	   if(time(0) - fillStartTime > timeLimitForFill) break;

  	   int p1 = processesCopy[i]->getId();

	   if(sol->getProcess(p1)->getMachine() == 0) continue;

       //for(unsigned int j = i + 1; j < sol->getNumberOfProcesses() && j < max_number_of_processes_to_try; j++){
	   for(unsigned int j = i + 1; j < processesCopy.size() && j < max_number_of_processes_to_try; j++){

     	   int p2 = processesCopy[j]->getId();

		   if(sol->getProcess(p2)->getMachine() == 0) continue;

		   int m1 = sol->getProcess(p1)->getMachine()->getId();
		   int m2 = sol->getProcess(p2)->getMachine()->getId();
		   if(m2 == m1) continue;
		   if(!sol->matrix_[p1][m2] || !sol->matrix_[p2][m1]) continue;

 	       if(sol->checkSwap(p1, p2))
 	       {
 	    	   double diff = sol->getCostDifferenceWithSwap(p1, p2);
			   if(diff < 0)
			   {
					  vector<int64> v(3);
					  v[1] = p1, v[2] = p2, v[0] = diff;
					  pairs.insert(v);

				   //stop if max_number_of_moves reached (B instances)
				   if(sol->getNumberOfProcesses() > 1000)
				   {
					   int max_number_of_moves = 50000;
					   if(sol->getNumberOfProcesses() < 5001)  max_number_of_moves = 53000;
					   if(pairs.size() > max_number_of_moves) return true;
				   }

			   }
 	       }

  	    }
 	}

	return true;
}





bool fillTheSetOfGoodShiftPairs(Solution* sol, Parameters& params,
		set<vector<int64>  >& pairs, int start_pos =  0, int end_pos =  50000)
{

	if(sol->getNumberOfProcesses() > 1000)
	  return fillTheSetOfGoodShiftPairsB(sol, params, pairs, start_pos, end_pos);


	for(int i = start_pos; i <= end_pos && i < sol->getNumberOfProcesses(); i++)
	{

	   int p = sol->getProcessAtPosition(i)->getId();

	   if(sol->getProcess(p)->getMachine() == 0) continue;

	   for(unsigned int m = 0; m < sol->getNumberOfMachines(); m++) {

		   if(!sol->matrix_[p][m]) continue;

 	       if(sol->checkShift(p, m))
 	       {
 	    	   double diff =  sol->getCostDifferenceWithShift(p, m);
			   if(diff < 0)
			   {
				   vector<int64> v(3);
				   v[1] = p, v[2] = m, v[0] = diff;
				   pairs.insert(v);

				   //stop if max_number_of_moves reached (B instances)
				   if(sol->getNumberOfProcesses() > 1000)
				   {
					   int max_number_of_moves = 50000;
					   if(sol->getNumberOfProcesses() < 5001)  max_number_of_moves = 75000;
					   if(pairs.size() > max_number_of_moves) return true;
				   }
		       }
 	       }

	   }
 	}

	return true;

}





bool fillTheSetOfGoodSwapPairs(Solution* sol, Parameters& params, set<vector<int64> >& pairs, int start_pos =  0, int end_pos =  50000)
{

	if(sol->getNumberOfProcesses() > 1000)
		return fillTheSetOfGoodSwapPairsB(sol, params, pairs, start_pos, end_pos);

    if(end_pos > sol->getNumberOfProcesses() - 1)
    	end_pos = sol->getNumberOfProcesses() - 1;

	vector<Process*> processesCopy2 = sol->getSortedProcesses();
	vector<Process*> processesCopy(processesCopy2.begin() + start_pos,processesCopy2.begin() + end_pos - start_pos + 1);

	for(unsigned int i = 0; i < processesCopy.size(); i++){

		int p1 = processesCopy[i]->getId();

		if(sol->getProcess(p1)->getMachine() == 0) continue;

		for(unsigned int j = i + 1; j < processesCopy.size(); j++){

			int p2 = processesCopy[j]->getId();

			if(sol->getProcess(p2)->getMachine() == 0) continue;

		   int m1 = sol->getProcess(p1)->getMachine()->getId();
		   int m2 = sol->getProcess(p2)->getMachine()->getId();
		   if(m2 == m1) continue;
		   if(!sol->matrix_[p1][m2] || !sol->matrix_[p2][m1]) continue;

 	       if(sol->checkSwap(p1, p2))
 	       {
 	    	   double diff = sol->getCostDifferenceWithSwap(p1, p2);
			   if(diff < 0)
			   {
					  vector<int64> v(3);
					  v[1] = p1, v[2] = p2, v[0] = diff;
					  pairs.insert(v);

				   //stop if max_number_of_moves reached (B instances)
				   if(sol->getNumberOfProcesses() > 1000)
				   {
					   int max_number_of_moves = 50000;
					   if(sol->getNumberOfProcesses() < 5001)  max_number_of_moves = 53000;
					   if(pairs.size() > max_number_of_moves) return true;
				   }

			   }
 	       }
  	    }
 	}

	return true;
}



// all feasible and good moves are saved in a vector
// random move is chosen from vector
// move is done if it is good and feasible
// if move is not good or not feasible it  is deleted from the vector
// vector is filled again when empty
// move is good if cost difference is negative
// only processes with position in interval [start_pos,end_pos] are considered
// max_number_of_moves is max number of shifts made
Solution* local_search_shift(Solution* sol, Parameters& params, int max_number_of_moves,
		             int start_pos, int end_pos, int max_number_of_fills)
{


	set<vector<int64> > good_pairs;

	//fill the set of good pairs
	fillTheSetOfGoodShiftPairs(sol, params, good_pairs, start_pos, end_pos);

	int64 cost_at_previous_fill = sol->getCost();

	while(good_pairs.size() > 0)
	{

		bool good_pair_doesnt_exist = false;

		 //choose random move from the set
		 vector<int64> random_pair = getRandomPair(good_pairs, params.numberOfBestProcessesToConsiderShiftSwap);
		 int p = (int) random_pair[1], m = (int) random_pair[2];

		 while(!sol->checkShift(p, m) || sol->getCostDifferenceWithShift(p,m) >= 0)
		 {

			 good_pairs.erase(random_pair);

			 if(good_pairs.size() == 0) {
				 good_pair_doesnt_exist = true;
				 break;
			 }

			 // choose random pair (p,m)
			random_pair = getRandomPair(good_pairs, params.numberOfBestProcessesToConsiderShiftSwap);
		    p = random_pair[1], m = random_pair[2];

		 }


		 if(good_pair_doesnt_exist)
		 {

			 if(time(0) - params.programStartTime > params.time_limit - 5) return sol;

			 // stop the search if improvement not significant (B  instances)
			 if(sol->getNumberOfProcesses() > 1000)
			 {
			     double improvement;
			     if(cost_at_previous_fill == 0) return sol;
			     improvement = ((double) (cost_at_previous_fill - sol->getCost())) / cost_at_previous_fill;
			     if(sol->getNumberOfProcesses() > 1000 && improvement < 0.0005)
			    	 return sol;
			     cost_at_previous_fill = sol->getCost();
			 }


			 if(--max_number_of_fills == 0) return sol;

			 //fill the set of good pairs
			 fillTheSetOfGoodShiftPairs(sol, params, good_pairs, start_pos, end_pos);


			 if(good_pairs.size() == 0) break;
		 }

		 if(!good_pair_doesnt_exist) {
			 sol->reassignProcess(p, m);
			 if(max_number_of_moves-- < 0) return sol;
		 }

	}

	return sol;

}



// all feasible and good moves are saved in a vector
// random move is chosen from vector
// move is done if it is good and feasible
// if move is not good or not feasible it  is deleted from the vector
// vector is filled again when empty
// move is good if cost difference is negative
Solution* local_search_swap(Solution* sol, Parameters& params, int max_number_of_moves,
		     int start_pos, int end_pos, int max_number_of_fills)
{


	set<vector<int64> > good_pairs;

	//fill the set of good pairs
	fillTheSetOfGoodSwapPairs(sol, params, good_pairs, start_pos, end_pos);

	int64 cost_at_previous_fill = sol->getCost();

	while(good_pairs.size() > 0)
	{

		bool good_pair_doesnt_exist = false;

		 //choose random move from the set
		 vector<int64> random_pair = getRandomPair(good_pairs, params.numberOfBestProcessesToConsiderShiftSwap);
		 int p1 = random_pair[1], p2 = random_pair[2];

		 while(!sol->checkSwap(p1, p2) ||
				sol->getCostDifferenceWithSwap(p1, p2) >= 0)
		 {

			 good_pairs.erase(random_pair);
			 if(good_pairs.size() == 0) {
				 good_pair_doesnt_exist = true;
				 break;
			 }

			 // choose random pair (p,m)
			random_pair = getRandomPair(good_pairs, params.numberOfBestProcessesToConsiderShiftSwap);
			p1 = (int) random_pair[1], p2 = (int) random_pair[2];

		 }

		 if(good_pair_doesnt_exist)
		 {

			 if(time(0) - params.programStartTime > params.time_limit - 5) return sol;

			 // stop the search if improvement not significant (B  instances)
			 if(sol->getNumberOfProcesses() > 1000)
			 {
				 double improvement;
				 if(cost_at_previous_fill == 0) return sol;
				 improvement = ((double) (cost_at_previous_fill - sol->getCost())) / cost_at_previous_fill;
				 if(sol->getNumberOfProcesses() > 1000 && improvement < 0.001) return sol;
				 cost_at_previous_fill = sol->getCost();
			 }

			 if(--max_number_of_fills == 0) return sol;

			 //fill the set of good pairs
			 fillTheSetOfGoodSwapPairs(sol, params, good_pairs, start_pos, end_pos);

			 if(good_pairs.size() == 0) break;
		 }

		 // (p1, p2) is good and feasible pair
		 if(!good_pair_doesnt_exist) {
			 int m1 = sol->getProcess(p1)->getMachine()->getId();
			 int m2 = sol->getProcess(p2)->getMachine()->getId();
			 sol->reassignProcess(p1, m2);
			 sol->reassignProcess(p2, m1);

			 if(max_number_of_moves-- < 0) return sol;

		 }

	}


	return sol;

}



bool sortCriteriaPosition(Process* p1, Process* p2)
{
	return p1->getPosition() < p2->getPosition();
}


bool tryToShiftBigProcess(Solution* sol, Process* big_process, int m, Parameters& params, int end_pos)
{


	int m0 = big_process->getMachineId();

	Machine* machine_to_assign_to = sol->getMachine(m);

	if(m == big_process->getMachine()->getId() || !sol->matrix_[big_process->getId()][m])      return false;

    int minProcesses = 5;
    if(end_pos < 1000) minProcesses = 0;

    if(sol->getMachine(m)->getNumberOfProcesses() <= minProcesses) return false;

    if(!sol->checkShiftWithNoCapacity(big_process, machine_to_assign_to)) return false;

	// start solution assignments (used for recovering current solution when change is "bad")
	vector<int> old_assignments = sol->getAssignmentsVectorCopy();

	int64 old_cost = sol->getCost();

	// reassign process from m0 to m and
    // shift other (some) processes from machine m
    // (process is shifted if solution is improved or not feasible)

       vector<Process*> processes_to_shift = machine_to_assign_to->getProcessesCopy();

       if(end_pos < 1000)
       {
    	   sort(processes_to_shift.begin(), processes_to_shift.end(), sortCriteriaPosition);
    	   int n = min(0, machine_to_assign_to->getNumberOfProcesses() - 1);
		   vector<Process*>::iterator it_ = processes_to_shift.begin();
		   for(int x = 0; x < n; x++) it_++;
		   if((*it_)->getPosition() > end_pos) return false;
       }

       sol->reassignProcess(big_process, machine_to_assign_to);

       vector<bool> dirty(sol->getNumberOfMachines(), false);

       // do not use matrix if instance too big
       // all machines are dirty
       if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() >= 20000 * 2000)
       {
    	   for(int k = 0; k < sol->getNumberOfMachines(); k++)
    		   dirty[k] = true;
       }


       dirty[m0] = true; dirty[m]  = true;
       int bestOf = 100;

	   vector<int> machineIndexes;
	   for(int k = 0; k < sol->getNumberOfMachines(); k++)
		   machineIndexes.push_back(k);


       // shift processes if change is "good"
	   for(int i = 0; i < processes_to_shift.size(); i++)
	   {

		   if(sol->getNumberOfProcesses() > 5000 && i > 10) break;

		   Process* p = processes_to_shift[i];
		   if(p->getMachine() != machine_to_assign_to) continue;

		   int best_machine_to_move_to = -1;
		   double best_cost = 1e15;

		   int numberOfFeasibleMoves = 0;

		   int64 costDiffWithRemoving = p->getMachine()->getCostDifferenceWithRemovingProcess(p);

		   // find best machine to shift process to
		   if(machineIndexes.size() > 100)
			   random_shuffle(machineIndexes.begin(), machineIndexes.end());

		   for(int k = 0; k < sol->getNumberOfMachines() && k < 100; k++)
		   {

			   int machine_index = machineIndexes[k];

			   if(sol->matrix_[p->getId()][machine_index] == false) continue;

			   bool checkShift;
			   if(dirty[machine_index])
				   checkShift = sol->checkShift(p->getId(), machine_index/*, params.capacity_tollerance*/);
			   else
				   checkShift = sol->shift_matrix_[machine_index][p->getId()] &&
				                sol->checkShiftWithNoCapacity(p->getId(), machine_index);

			   if( checkShift)
			   {

				   int64 costDiff = costDiffWithRemoving;
    			   if(dirty[machine_index])
    				   costDiff += sol->getMachine(machine_index)->getCostDifferenceWithAddingProcess(p);
    			   else
    				   costDiff += sol->cost_difference_matrix_[machine_index][p->getId()];

    			   costDiff += sol->getPMCDifferenceWithShift(p, sol->getMachine(machine_index));
    			   costDiff += sol->getSMCDifferenceWithShift(p, sol->getMachine(machine_index));
    			   costDiff += sol->getMMCDifferenceWithShift(p, sol->getMachine(machine_index));

                   if(costDiff < best_cost)
				   {
					   best_cost = costDiff;
					   best_machine_to_move_to = machine_index;
				   }

				   numberOfFeasibleMoves++;
				   if(numberOfFeasibleMoves == bestOf) break;

			   }

		   }

		   if(best_machine_to_move_to >= 0 && best_cost < 0)
		   {
			   sol->reassignProcess(p->getId(), best_machine_to_move_to);
		       dirty[best_machine_to_move_to] = true;
		   }

	   }


       // recover original solution if new cost is bigger
	   if(sol->getCost() >= old_cost)
		{
	    	sol->setAssignments(old_assignments);
    	    return false;
		}


       // shift processes until solution is feasible
	   for(int i = 0; i < processes_to_shift.size(); i++)
	   {

		   if(sol->getNumberOfProcesses() > 5000 && i > 10) break;

		   Process* p = processes_to_shift[i];
		   if(p->getMachine() != machine_to_assign_to) continue;

		   int best_machine_to_move_to = -1;
		   double best_cost = 1e15;

		   int numberOfFeasibleMoves = 0;

		   int64 costDiffWithRemoving = p->getMachine()->getCostDifferenceWithRemovingProcess(p);

		   // find best machine to shift process to

		   if(machineIndexes.size() > 100)
			   random_shuffle(machineIndexes.begin(), machineIndexes.end());

		   for(int k = 0; k < sol->getNumberOfMachines() && k < 100; k++)
		   {

			   int machine_index = machineIndexes[k];

			   if(sol->matrix_[p->getId()][machine_index] == false) continue;

			   bool checkShift;
			   if(dirty[machine_index])
				   checkShift = sol->checkShift(p->getId(), machine_index);
			   else
				   checkShift = sol->shift_matrix_[machine_index][p->getId()] &&
				                sol->checkShiftWithNoCapacity(p->getId(), machine_index);


			   if( checkShift)
			   {

				   int64 costDiff = costDiffWithRemoving;
    			   if(dirty[machine_index])
    				   costDiff += sol->getMachine(machine_index)->getCostDifferenceWithAddingProcess(p);
    			   else
    				   costDiff += sol->cost_difference_matrix_[machine_index][p->getId()];

    			   costDiff += sol->getPMCDifferenceWithShift(p, sol->getMachine(machine_index));
    			   costDiff += sol->getSMCDifferenceWithShift(p, sol->getMachine(machine_index));
    			   costDiff += sol->getMMCDifferenceWithShift(p, sol->getMachine(machine_index));


                   if(costDiff < best_cost)
				   {
					   best_cost = costDiff;
					   best_machine_to_move_to = machine_index;
				   }

				   numberOfFeasibleMoves++;
				   if(numberOfFeasibleMoves == bestOf) break;

			   }


		   }

		   if(best_machine_to_move_to >= 0 && (best_cost < 0 || !sol->checkCapacity()))
		   {
			   sol->reassignProcess(p->getId(), best_machine_to_move_to);
		       dirty[best_machine_to_move_to] = true;
		   }

	   }


	    // if processes could not be shifted recover original solution
       // recover original solution if new cost is bigger
	   if( !sol->checkCapacity() || sol->getCost() >= old_cost)
        {
     	   sol->setAssignments(old_assignments);
     	   return false;
        }




    // do not update matrix if instance too big
    if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() >= 20000 * 2000)
      return true;


    // update shift and cost matrix

    for(int ii = 0; ii < sol->getNumberOfMachines(); ii++)
      if(dirty[ii])
      {
    	    for(int pp = 0; pp < sol->getNumberOfProcesses(); pp++) {
    	    	sol->shift_matrix_[ii][pp] =
    	    			sol->checkCapacityWithShift(sol->getProcess(pp), sol->getMachine(ii));

    	    	sol->cost_difference_matrix_[ii][pp] =
    	       			sol->getMachine(ii)->getCostDifferenceWithAddingProcess(sol->getProcess(pp));
    	    }
      }

    return true;

}


bool biggerLoadCost(Machine* m1, Machine* m2)
{
	return m1->getLoadCost() > m2->getLoadCost();
}


// big_process_rearrangement
void big_process_rearrangement(Solution *sol, Parameters& params, int start_pos, int end_pos)
{

	PARAMS_ = params;

    // choose big process to move
	Process* big_process = 0;
	if(end_pos != 50000) {    // range given
		if(end_pos > sol->getNumberOfProcesses() - 1) end_pos = sol->getNumberOfProcesses() - 1;
		big_process = sol->getProcessAtPosition(start_pos + rand() % (end_pos - start_pos + 1));
	}
	else {
		int nmbMachinesToChooseFrom = 4;
		vector<Machine*> machinesCopy_ = sol->getMachinesCopy();
		sort(machinesCopy_.begin(), machinesCopy_.end(), biggerLoadCost);
		int rand_machine = rand() % nmbMachinesToChooseFrom;
		while(machinesCopy_[rand_machine]->getNumberOfProcesses() == 0)
			rand_machine = rand() % nmbMachinesToChooseFrom;
		big_process = machinesCopy_[rand_machine]->getProcess(rand() % machinesCopy_[rand_machine]->getNumberOfProcesses());
	}


    // find machine to assign big process to
	// check all machines until "good" machine is find
	vector<Machine*> machinescopy = sol->getMachinesCopy();
	if(machinescopy.size() > 100) random_shuffle(machinescopy.begin(), machinescopy.end());

    int upper = min(100, sol->getNumberOfMachines());

    for(int m = 0; m < upper; m++)
    {
		bool shifted =
				tryToShiftBigProcess(sol, big_process, machinescopy[m]->getId(), params, end_pos);
        if(shifted) return;
    }


}
