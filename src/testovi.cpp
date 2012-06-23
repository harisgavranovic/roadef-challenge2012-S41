
#include "testovi.h"
using namespace std;


void readSolutionFromFile(Solution* sol, string filename)
{
	fstream solutionFile(filename.c_str(), ios_base::in);

	for(int i = 0; i < sol->getNumberOfProcesses(); i++)
	{
		int m;
		solutionFile >> m;
		sol->reassignProcess(sol->getProcess(i), sol->getMachine(m));
	}
}


// decrease machine capacities and set process requirements
// to zero if process can be assigned to one machine only
Data* preprocesData(Solution* sol)
{

	  Data* data = sol->getData();

	  int number_of_removed_processes = 0;

	  while(sol->getNumberOfProcessesThatCanBeAssignedToOnlyOneMachine() > 0)
	  {

		  for(int p = 0; p < data->getNumberOfProcesses(); p++)
		  {
			  int n = sol->getNumberOfMachinesProcessCanBeAssignedTo(p);
			  if(n == 1)
			  {

				  Process* process = data->getProcess(p);

				  int machine_index = -1;
				  for(int m = 0; m < sol->getNumberOfMachines(); m++)
					if(sol->matrix_[p][m] == true) {
						machine_index =  m;
						break;
					}

				   Machine* machine = data->getMachine(machine_index);

				   //decrease capacities
				   for(int r = 0; r < data->getNumberOfResources(); r++)
				   {

					   machine->getMachineResource(r)->decreaseCapacity(process->getRequirement(r));
					   machine->getMachineResource(r)->decreaseSafetyCapacity(process->getRequirement(r) );

				 	   data->getResource(r)->setTotalCapacity(
				 			  data->getResource(r)->total_capacity  - process->getRequirement(r));
				 	   data->getResource(r)->setTotalRequirement(
				 			  data->getResource(r)->total_requirement - process->getRequirement(r));
				 	   data->getResource(r)->setTotalSafetyCapacity(
				 			  data->getResource(r)->total_safety_capacity - process->getRequirement(r));


					   process->setRequirement(r, 0);

					   number_of_removed_processes++;
				   }



				   for(int p2_i = 0; p2_i < process->getService()->getNumberOfProcesses(); p2_i++)
				   {
					   int p2 = process->getService()->getProcess(p2_i)->getId();
					   int n2 = sol->getNumberOfMachinesProcessCanBeAssignedTo(p2);
					   if(n2 >= 2)
					   {
						   if(sol->matrix_[p2][machine_index] &&
						      sol->getProcess(p2)->getService() == sol->getProcess(p)->getService())
						   {
							   sol->matrix_[p2][machine_index] = false;
						   }
					   }
				   }
			  }
		  }

          sol->fillMatrix();

          for(int p = 0; p < sol->getNumberOfProcesses(); p++)
          {
        	  bool empty_process = true;
        	  for(int r = 0; r < data->getNumberOfResources(); r++)
        		  if(sol->getProcess(p)->getRequirement(r) > 0)
        		  {
        			  empty_process = false;
        			  break;
        		  }
        	  if(empty_process)
        	  {
        		  for(int m = 0; m < sol->getNumberOfMachines(); m++)
        			 sol->matrix_[p][m] = false;
        	  }
          }
	  }


      for(int p = 0; p < sol->getNumberOfProcesses(); p++)
   		  sol->matrix_[p][sol->getProcess(p)->getInitialMachine()->getId()] = true;

	  return data;

}




Solution* constructSolution(Parameters& params)
{
      if(params.data_filename == "") exit(0);
	  Solution* sol = new Solution();
	  readDataFromFilesAndConstructSolution(params.data_filename,
											params.initial_assignment_filename,
											params.new_assignment_filename, sol );


	  ostringstream oss("");
	  oss << "solution_" << params.ID;
	  sol->setSolutionFilename(oss.str());
	  return sol;
}



void BPR(Solution* sol, Parameters& params, int iterations, int start_pos, int end_pos)
{

	if(iterations == 0) return;
    int startTime = time(0);

    // do not update matrix if instance too big
    if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() < 20000 * 2000)
    	sol->fillShiftMatrix();

    for(int iter = 0; iter < iterations; iter++)
    {
    	if(time(0) - params.programStartTime > params.time_limit - 5) return;
    	big_process_rearrangement( sol, params, start_pos, end_pos);
    }

}


Solution* constructSolutionForMultipliedInstance(Parameters& params, int number_of_multiplies)
{
	return constructSolution(params);
}


bool absPosition(Process* p1, Process* p2)
{
	return p1->getPosition() < p2->getPosition();
}



//sort resources by score (respect to the lower bound)
vector<int> sortResources(Solution* sol)
{

		vector<int> resources_sorted;
		for(int r = 0; r < sol->getNumberOfResources(); r++) resources_sorted.push_back(r);

		// sort resources
		while(true)
   	    {
		    bool swapped = false;
		    for (int i = 0 ; i < resources_sorted.size() - 1; i++)
		    {

		       double value1, value2;

		       value1  = sol->getloadCostForResource(resources_sorted[i]) - sol->getData()->getLoadCostLB(resources_sorted[i]);

		       value2  = sol->getloadCostForResource(resources_sorted[i+1]) - sol->getData()->getLoadCostLB(resources_sorted[i+1]);

		       if (value2 > value1 )
		       {
		           int x = resources_sorted[i];
		    	   resources_sorted[i] = resources_sorted[i + 1];
		    	   resources_sorted[i + 1] = x;
		           swapped = true;
		       }
		    }
		    if(!swapped) break;
		}

		return resources_sorted;

}





int64 solveRangeA(Solution* sol, Parameters& params, int startPos, int endPos, bool changedWeights = false)
{

	    if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

	    int numProcesses = sol->getNumberOfProcesses();
	    int num_processes_in_first_iteration = 50 * numProcesses / 1000;
	    int delta = params.delta * numProcesses / 1000;
		int nmb_iters_bpr = params.nmb_iters_bpr;

	    int repeatsEnd = round( ((double)(endPos - num_processes_in_first_iteration)) / delta ) + 1 ;
	    int repeatsBegin = round( ((double)(startPos - num_processes_in_first_iteration)) / delta ) ;
	    if(startPos == 0) repeatsBegin = 0;

		for (int i = repeatsBegin; i < repeatsEnd; i++)
		{

			int end = num_processes_in_first_iteration + i * delta ;
			int start = num_processes_in_first_iteration + repeatsBegin * delta;
			if(repeatsBegin == 0) start = 0;
			if(start == end) continue;

			if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();
			BPR(sol, params, nmb_iters_bpr, 0, end);


			if(!changedWeights) {
				sol->updateBestCost();
			}

			if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

			local_search_shift(sol, params, 100000, 0, end);
			local_search_swap (sol, params, 100000, 0, end);

			if(!changedWeights) {
				sol->updateBestCost();
			}

	    }

		return sol->getCost();

}



int64 solveWithChangingLoadCostWeightsA(Solution* sol, Parameters& params, int start_pos, int end_pos)
{


	  int64 bestCost = sol->getCost();
	  vector<int> bestAssignments = sol->getAssignmentsVectorCopy();
	  sol->updateBestCost();

	  if(end_pos > sol->getNumberOfProcesses())
		  end_pos = sol->getNumberOfProcesses();

	  int numberOfProcessesInRange = params.rangeLength;

	  // set start and end position for range
	  int startPos = start_pos;
	  int endPos = min(end_pos, startPos + numberOfProcessesInRange);

	  // solve for every range
	  for(;;)
	  {

		  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

		  //sort resources
		  vector<int> resources_sorted;
		  for(int r = 0; r < sol->getNumberOfResources(); r++) resources_sorted.push_back(r);
		  resources_sorted = sortResources(sol);

		  for(int r_i = 0; r_i < sol->getNumberOfResources() / 2 + 1; r_i ++)
		  {

			      int r;
				  if(r_i == sol->getNumberOfResources() / 2) r = r_i;
				  else         r = resources_sorted[r_i];

				  //*******************************solve original instance****************************
				  sol->setOriginalLoadCostWeights();

				  int64 currentCost = solveRangeA(sol, params, startPos, endPos);

				  if(currentCost < bestCost) {

					  bestCost = currentCost;
					  bestAssignments = sol->getAssignmentsVectorCopy();
					  sol->updateBestCost();

				  }
				  //************************************************************************************


				  if(r_i == sol->getNumberOfResources() / 2) break;
				  if(time(0) - params.programStartTime > params.time_limit) break;


				  //**********************solve with resource priority***********************************
				  sol->setAssignments(bestAssignments);

				  sol->changeLoadCostWeights(r, 10);

				  currentCost = solveRangeA(sol, params, startPos, endPos, true);

				  sol->setOriginalLoadCostWeights();
				  sol->updateBestCost();
				  //*************************************************************************************

		  }

		  if(endPos >= end_pos) break;

		  //// set start and end position for the next range
		  startPos += numberOfProcessesInRange;
		  endPos += numberOfProcessesInRange;
		  if(endPos > end_pos) endPos = end_pos;

	  }

      sol->setAssignments(bestAssignments);

      return bestCost;

}


int64 solveAFinal(Parameters& params, Solution* startSol)
{

  //**********************Read Data and Set Some Parameters******************
  if(params.data_filename == "") exit(0);
  int64 startTime = time(0);
  srand(params.seedValue);
  Solution* sol = new Solution();
  if(startSol != 0) sol = startSol;
  else
	  readDataFromFilesAndConstructSolution(params.data_filename,
					params.initial_assignment_filename,
					params.new_assignment_filename, sol );
  //*************************************************************************



	vector<int> startAssignments = sol->getAssignmentsVectorCopy();

    int64 SeeD = params.seedValue;
	int64 BEST = sol->getCost();

	// while there is enough time
	int solution_counter = 0;
	while(time(0) - params.programStartTime < params.time_limit)
	{

		vector<int> nmbSolutions;
		vector<int> rangeLenghts(4, params.rangeLength);
		vector<int> timeLimits;

		nmbSolutions.push_back(15); nmbSolutions.push_back(8); nmbSolutions.push_back(4); nmbSolutions.push_back(2);
		timeLimits.push_back(90 * params.time_limit / 300);
		timeLimits.push_back(80 * params.time_limit / 300);
		timeLimits.push_back(30 * params.time_limit / 300);
		timeLimits.push_back(50 * params.time_limit / 300);

		set<pair<int64,vector<int> > > costsAndAssignmentsPrevious;
		int64 currentCost = sol->getCost();
		for(int i = 0; i < nmbSolutions[0]; i++)
		   costsAndAssignmentsPrevious.insert(make_pair(currentCost++,startAssignments));
	    set<pair<int64,vector<int> > > costsAndAssignmentsNew;

	    int startPos = 0;
	    int endPos = rangeLenghts[0];

	    for(unsigned int i = 0; i < nmbSolutions.size(); i++)
	    {

	      int64 startI = time(0);

		  costsAndAssignmentsNew.clear();

		  for(int j = 0; j < nmbSolutions[i]; j++)
	      {

			  // solve best solution with two seeds
			  int nmbSeeds = 1;
			  if(j == 0 && i > 0) nmbSeeds = 2;

			  for(int z = 0; z < nmbSeeds; z++ )
			  {

				   srand(SeeD++);

				   set<pair<int64,vector<int> > >::iterator it = costsAndAssignmentsPrevious.begin();
				   for(int k = 0; k < j; k++) it++;
				   sol->setAssignments((*it).second);

				   int64 currentCost = solveWithChangingLoadCostWeightsA(sol, params, startPos, endPos);
				   sol->updateBestCost();

				   costsAndAssignmentsNew.insert(make_pair(currentCost, sol->getAssignmentsVectorCopy()));

				   if(time(0) - startI > timeLimits[i]) break;

			   }

			   if(time(0) - startI > timeLimits[i]) break;

	        }

		    if(i < rangeLenghts.size() - 1)
		    {
				startPos = endPos;
				endPos += rangeLenghts[i + 1];

				if(nmbSolutions[i + 1] > costsAndAssignmentsNew.size())
					nmbSolutions[i + 1] = costsAndAssignmentsNew.size();

		    }

		    costsAndAssignmentsPrevious = costsAndAssignmentsNew;

	    }

		set<pair<int64,vector<int> > >::iterator it = costsAndAssignmentsPrevious.begin();
		sol->setAssignments((*it).second);

		currentCost = solveWithChangingLoadCostWeightsA(sol, params, endPos, sol->getNumberOfProcesses() - 1);

		if(currentCost < BEST)
		{
			  BEST = currentCost;
			  sol->updateBestCost();
		 }

		solution_counter++;

		int averageSolutionTime = (time(0) - startTime) / solution_counter;
		if(time(0) - startTime + averageSolutionTime > params.time_limit) break;
		if(time(0) - startTime > params.time_limit) break;

	}

	return BEST;

}




// B, X instances


int64 solveRangeB(Solution* sol, Parameters& params, int start_pos, int end_pos,
		int64 rangeStartTime, int64 timeLimitForRange, bool changedWeights = false)
{


	 // bpr
	 if(params.nmb_iters_bpr > 0)
	 {
		 for(int ii = 0; ii < 5; ii++)
		 {
			  double costBefore = sol->getCost();
			  int64 startBPR = time(0);

			  if(!changedWeights)
				  sol->updateBestCost();

			  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

			  BPR(sol, params, params.nmb_iters_bpr / 5);

			  if(!changedWeights)
				  sol->updateBestCost();

			  if(((double) (costBefore - sol->getCost())) / costBefore < 0.005) break;
			  if(time(0) - rangeStartTime > timeLimitForRange) break;
			  if(time(0) - startBPR > 5 * params.time_limit / 300) break;
		 }
	 }

	 // ls shift + swap
	 if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();
	 local_search_shift(sol, params, 1000000, 0, end_pos);

	 if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();
	 local_search_swap (sol, params, 1000000, 0, end_pos);

	 if(!changedWeights)
		 sol->updateBestCost();

	 return sol->getCost();

}


int64 solveWithChangingLoadCostWeightsB(Solution* sol, Parameters& params, int start_pos, int end_pos, int64 timeLimit = -1)
{
	  int64 bestCost = sol->getCost();
	  vector<int> bestAssignments = sol->getAssignmentsVectorCopy();
	  sol->updateBestCost();


	  if(end_pos > sol->getNumberOfProcesses())
		  end_pos = sol->getNumberOfProcesses();

	  int numberOfProcessesInRange = params.rangeLength;


	  int timeLimitForRange;
	  if(timeLimit < 0) {
		  timeLimit = params.time_limit;
	      timeLimitForRange = (4 * timeLimit / 5) * numberOfProcessesInRange / sol->getNumberOfProcesses();
	  }
	  else
		  timeLimitForRange = timeLimit;

	  // set start and end position for range
	  int startPos = start_pos;
	  int endPos = min(end_pos, startPos + numberOfProcessesInRange);

	  // solve for every range
	  for(;;)
	  {

		  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

		  int64 rangeStartTime = time(0);

		  //sort resources
		  vector<int> resources_sorted;
		  for(int r = 0; r < sol->getNumberOfResources(); r++) resources_sorted.push_back(r);
		  resources_sorted = sortResources(sol);

		  int nmbResourcesToUse = max(sol->getNumberOfResources() / 2, 3);
		  if(sol->getNumberOfResources() < nmbResourcesToUse)
			  nmbResourcesToUse = sol->getNumberOfResources();

		  for(int L = 0; L < params.nmbLoops; L++)
		  for(int r_i = 0; r_i < nmbResourcesToUse + 1; r_i ++)
		  {
			     if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

				  int r;
				  if(r_i == nmbResourcesToUse) r = r_i;
				  else         r = resources_sorted[r_i];

				  //*******************************solve original instance****************************
				  sol->setOriginalLoadCostWeights();

				  if(time(0) - rangeStartTime > timeLimitForRange) break;

				  int64 currentCost = solveRangeB(sol, params, startPos, endPos, rangeStartTime, timeLimitForRange);

				  if(currentCost < bestCost) {
					  bestCost = currentCost;
					  bestAssignments = sol->getAssignmentsVectorCopy();
					  sol->updateBestCost();
				  }
				  //************************************************************************************


				  if(r_i == nmbResourcesToUse) break;
				  if(time(0) - params.programStartTime > params.time_limit) break;
				  if(sol->getloadCostForResource(r) - sol->getData()->getLoadCostLB(r) < 0.1) break;

		          double value  = sol->getloadCostForResource(r) - sol->getData()->getLoadCostLB(r);
		          value /=  (sol->getloadCostForResource(r) + 1);

				  if( value < 0.01) break;

				  //**********************solve with resource priority***********************************
				  sol->setAssignments(bestAssignments);

				  sol->changeLoadCostWeights(r, 10);

				  if(time(0) - rangeStartTime > timeLimitForRange) break;
				  currentCost = solveRangeB(sol, params, startPos, endPos, rangeStartTime, timeLimitForRange, true);

				  sol->setOriginalLoadCostWeights();
				  sol->updateBestCost();
				  //*************************************************************************************

		  }

		  sol->setOriginalLoadCostWeights();

		  if(endPos >= end_pos) break;

		  startPos += numberOfProcessesInRange;
		  endPos += numberOfProcessesInRange;
		  if(endPos > end_pos) endPos = end_pos;

	  }

	  sol->setOriginalLoadCostWeights();
      sol->setAssignments(bestAssignments);

      return bestCost;

}



// solve a given range iter few times and sort seeds by objective value
vector<vector<int> > findBestSeedsB(Solution* sol, Parameters& params, int numberOfSolutions, int64 timeLimit,  int start_pos, int end_pos)
{

	  int64 startTime = time(0);
	  set<pair<int64,int64> > seedsAndCosts;
	  vector<int> startAssignments = sol->getAssignmentsVectorCopy();

	  map<int64, vector<int> > seedAndAssignments;

	  int seed = rand();

	  // solve numberOfSolutions times
	  for(int s = 0; s < numberOfSolutions; s++)
	  {

		  if(time(0) - startTime > timeLimit) break;
		  seed++; srand(seed);
		  sol->setAssignments(startAssignments);

		  int64 currentCost = solveWithChangingLoadCostWeightsB(sol, params, start_pos, end_pos);

		  sol->setOriginalLoadCostWeights();
		  sol->updateBestCost();

		  seedsAndCosts.insert(make_pair(currentCost, seed));
		  seedAndAssignments.insert(make_pair(seed, sol->getAssignmentsVectorCopy()));

		  int averageSolutionTime = (time(0) - startTime) / (s + 1);

		  // break if there is not enough time for next seed
		  // or if there is not enough time for 3 seeds at least
		  if(time(0) - startTime + averageSolutionTime > timeLimit) break;
		  if(time(0) - startTime + (2 - s) * averageSolutionTime > timeLimit) break;

	    }

	    vector<vector<int> > assignments;
	    for(set<pair<int64,int64> >::iterator it = seedsAndCosts.begin(); it != seedsAndCosts.end(); it++)
	    {
		    assignments.push_back(seedAndAssignments.find((*it).second)->second);
	    }

	    sol->setAssignments(assignments[0]);

	  return assignments;
}


int64 solveBFinal(Parameters& params, Solution* startSol)
{

	  //**********************Read Data and Set Some Parameters******************
	  if(params.data_filename == "") exit(0);
	  int64 startTime = time(0);
	  srand(params.seedValue);
	  Solution* sol = new Solution();
	  if(startSol != 0) sol = startSol;
	  else
		  readDataFromFilesAndConstructSolution(params.data_filename,
				    	params.initial_assignment_filename,
				    	params.new_assignment_filename, sol );
	  //*************************************************************************


	  vector<int> startAssignments = sol->getAssignmentsVectorCopy();


	  //****************************Find Best Seeds********************************************
	  int timeLimitForSeed = 120 * params.time_limit / 300, maxNumberOfSeeds = 20;
	  int startPositionForSeed = 0, endPositionForSeed = params.rangeLength;


	  vector<vector<int> > assignments = findBestSeedsB(sol, params, maxNumberOfSeeds,
			              timeLimitForSeed, startPositionForSeed, endPositionForSeed);
	  //***************************************************************************************

	  int64 timeConsumedForSeeds = time(0) - params.programStartTime;

	  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

	  //**************************solve for few best seeds**************************************
      // solve for few best seeds
      int64 bestOfAll = sol->getCost();
      vector<int> bestAssignmentsOfAll = sol->getAssignmentsVectorCopy();
      vector<int64> costs;
      int nmbSolutions = 10;
      if(assignments.size() < nmbSolutions) nmbSolutions = assignments.size();
      int64 seeD = params.seedValue;
      for(int i = 0; i < nmbSolutions; i++)
      {

    	  if(time(0) - params.programStartTime > params.time_limit - 5) return bestOfAll;

    	  srand(seeD++);

    	  sol->setAssignments(assignments[i]);


    	  int64 currentCost = solveWithChangingLoadCostWeightsB(sol, params,
				  endPositionForSeed, sol->getNumberOfProcesses());

		  if(currentCost < bestOfAll) {
			  bestOfAll = currentCost;
			  bestAssignmentsOfAll = sol->getAssignmentsVectorCopy();
			  sol->updateBestCost();

		  }

		  costs.push_back(currentCost);

		  int averageSolutionTime = (time(0) - startTime - timeConsumedForSeeds) / (i + 1);
		  if(time(0) - startTime + averageSolutionTime > params.time_limit ) break;
		  if(time(0) - startTime > params.time_limit) break;
      }
      //***************************************************************************************

	  sol->setAssignments(bestAssignmentsOfAll);

	  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

	  //*****************************solve 0-50000 again*******************************
	  params.rangeLength = 50000;
	  int64 currentCost = solveWithChangingLoadCostWeightsB(sol, params, 0, 50000,
			  params.time_limit - (time(0) - startTime));
	  sol->updateBestCost();
	  //*******************************************************************************


	  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

	  //*****************************random LS for remaining time*************************
	  while(time(0) - startTime < params.time_limit)
	  {
		  local_search_shift_random(sol, params);
		  sol->updateBestCost();

		  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

		  local_search_swap_random(sol, params);
		  sol->updateBestCost();

		  if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

		  local_search_shift(sol, params, 1000000, 0, 50000, 1);
		  sol->updateBestCost();

		  //cout << sol->getCost() << endl;

	  }
	  //**********************************************************************************

	  return sol->getCost();

}



int64 solveBFinal2(Parameters& params, Solution* startSol)
{


  //**********************Read Data and Set Some Parameters******************
  if(params.data_filename == "") exit(0);
  int64 startTime = time(0);
  srand(params.seedValue);
  Solution* sol = new Solution();
  if(startSol != 0) sol = startSol;
  else
	  readDataFromFilesAndConstructSolution(params.data_filename,
					params.initial_assignment_filename,
					params.new_assignment_filename, sol );
  //*************************************************************************

	vector<int> startAssignments = sol->getAssignmentsVectorCopy();

    int64 SeeD = params.seedValue;
	int64 BEST = sol->getCost();

	// while there is enough time
	int solution_counter = 0;
	while(time(0) - params.programStartTime < params.time_limit)
	{

		vector<int> nmbSolutions;
		vector<int> timeLimits;
		vector<int> nmbSeeds;
		vector<int> rangeLenghts(4, params.rangeLength);

		nmbSolutions.push_back(8); nmbSolutions.push_back(1); nmbSolutions.push_back(1); nmbSolutions.push_back(1);

		timeLimits.push_back(70);  timeLimits.push_back(60);  timeLimits.push_back(50);  timeLimits.push_back(50);

		nmbSeeds.push_back(1); nmbSeeds.push_back(2); nmbSeeds.push_back(1); nmbSeeds.push_back(1);

		set<pair<int64,vector<int> > > costsAndAssignmentsPrevious;
		int64 currentCost = sol->getCost();
		for(int i = 0; i < nmbSolutions[0]; i++)
		   costsAndAssignmentsPrevious.insert(make_pair(currentCost++,startAssignments));
	    set<pair<int64,vector<int> > > costsAndAssignmentsNew;

	    int startPos = 0;
	    int endPos = rangeLenghts[0];

	    for(unsigned int i = 0; i < nmbSolutions.size(); i++)
	    {

	      if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

	      int64 startI = time(0);

		  costsAndAssignmentsNew.clear();
		  for(int l = 0; l < nmbSeeds[i]; l++) {
		  for(int j = 0; j < nmbSolutions[i]; j++)
	      {

			   if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

			   srand(SeeD++);

		       set<pair<int64,vector<int> > >::iterator it = costsAndAssignmentsPrevious.begin();
		       for(int k = 0; k < j; k++) it++;
		       sol->setAssignments((*it).second);

		       int64 currentCost;

		       if(i < 1)
		           currentCost = solveWithChangingLoadCostWeightsB(sol, params, startPos, endPos);
		       else {
		    	   if(sol->getNumberOfProcesses() <= 5000)
		    		   currentCost = solveWithChangingLoadCostWeightsA(sol, params, startPos, endPos);
		    	   else
		    	      currentCost = solveWithChangingLoadCostWeightsB(sol, params, startPos, endPos);
		       }

		       sol->setOriginalLoadCostWeights();
		       sol->updateBestCost();


		       costsAndAssignmentsNew.insert(make_pair(currentCost, sol->getAssignmentsVectorCopy()));

			   int averageI = (time(0) - startI) / (j + 1);
		       if(time(0) - startI + averageI > timeLimits[i]) break;

	        }

		    if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();
		    if(time(0) - startI > timeLimits[i]) break;

		    }


		    if(i < rangeLenghts.size() - 1)
		    {
				startPos = endPos;
				endPos += rangeLenghts[i + 1];

				if(nmbSolutions[i + 1] > costsAndAssignmentsNew.size())
					nmbSolutions[i + 1] = costsAndAssignmentsNew.size();

		    }

		    costsAndAssignmentsPrevious = costsAndAssignmentsNew;

	    }


	    if(time(0) - params.programStartTime > params.time_limit - 5) return sol->getCost();

		srand(SeeD++);

		set<pair<int64,vector<int> > >::iterator it = costsAndAssignmentsPrevious.begin();
		sol->setAssignments((*it).second);

		  if(sol->getNumberOfProcesses() <= 5000)
			currentCost = solveWithChangingLoadCostWeightsA(sol, params, endPos, sol->getNumberOfProcesses() - 1);
		  else
			currentCost = solveWithChangingLoadCostWeightsB(sol, params, endPos, sol->getNumberOfProcesses() - 1);

		if(currentCost < BEST)
		{
			  BEST = currentCost;
			  sol->updateBestCost();

		}

		solution_counter++;

		int averageSolutionTime = (time(0) - startTime) / solution_counter;
		if(time(0) - startTime + averageSolutionTime > params.time_limit) break;
		if(time(0) - startTime > params.time_limit) break;

	}

	return BEST;

}


