#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <iomanip>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "testovi.h"
#include "parameters.h"
#include "IO.h"
#include "forkic.h"




/***************************PROGRAMM PARAMETERS*********************************************

 -t     time_limit
 -p     instance_filename to load the data associated with the instance instance_filename
 -i     original_solution_filename to designate the file with the reference solution
 -o     new_solution_filename to designate the result file.
 -name  to return the identifier of the team that is the author of the executable
 -s     seed to force program with random to be deterministic
 -n     new solution

*******************************************************************************************/



int main(int argc, char **argv)
{

	  //*****************************SET PARAMETERS*************************************************
	  Parameters params(argc, argv, 0);
	  Parameters params2(argc, argv, 1);
	  //********************************************************************************************

	  //**********************READ DATA AND MAKE SOLUTION*******************************************
	  Solution* sol = constructSolution(params);
	  preprocesData(sol);
	  Solution* sol2 = constructSolution(params2);
	  preprocesData(sol2);
	 //*********************************************************************************************


	  //*****************************SET PARAMETERS*************************************************
	  if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() <= 100 * 1000) {
		  params2.delta = params.delta = 40;
		  params2.nmb_iters_bpr = params.nmb_iters_bpr = 300;
		  params2.nmbRanges = params.nmbRanges = 7;
		  params2.rangeLength = params.rangeLength = sol->getNumberOfProcesses() / params.nmbRanges;
	  }
	  else {
		  params2.nmb_iters_bpr = params.nmb_iters_bpr = 100;
		  params2.nmbRanges = params.nmbRanges = 5;
		  params2.rangeLength = params.rangeLength = sol->getNumberOfProcesses() / params.nmbRanges;
		  params2.numberOfBestProcessesToConsiderShiftSwap =
				  params.numberOfBestProcessesToConsiderShiftSwap = 3;
		  params2.nmbLoops = params.nmbLoops = 2;

		  if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() >= 20000 * 2000)
			  params2.nmbLoops = params.nmbLoops = 1;
	  }

	  params2.seedValue = params.seedValue + 1000;
	  //********************************************************************************************


	  if(sol->getNumberOfProcesses() * sol->getNumberOfMachines() <= 100 * 1000)  // A instances
		  solveA(params, sol, params2, sol2);
	  else  																	  // B instances
		  solveB(params, sol, params2, sol2);

	  long timeToSleep = params.time_limit - (time(0) - params.programStartTime) - 3;
	  if (timeToSleep < 0)
		  timeToSleep = 0;

	  sleep(timeToSleep);

	  // Read Solutions From Files And Write a better one

	  sol->setOriginalLoadCostWeights();
	  sol2->setOriginalLoadCostWeights();
	  //cout << " sol->solutionFilename   " << sol->solutionFilename << endl;
	  //cout << " sol2->solutionFilename  " << sol2->solutionFilename << endl;

	  //cout << " sol->getCost()   " << sol->getCost() << endl;
	  //cout << " sol2->getCost()  " << sol2->getCost() << endl;

	  readSolutionFromFile( sol,  sol->solutionFilename);
	  readSolutionFromFile(sol2, sol2->solutionFilename);


	  //cout << " after sol->getCost()   " << sol->getCost() << endl;
	  //cout << " after sol2->getCost()  " << sol2->getCost() << endl;
	  if ((sol->getCost() < sol2->getCost()) && (sol->checkConflict(false)))
	    sol->writeToFile(params.solution_filename);
	  else
		sol2->writeToFile(params.solution_filename);


	  //remove solution files
	  ostringstream oss1(""); oss1 << "rm " << sol->solutionFilename;
	  system(oss1.str().c_str());
	  ostringstream oss2(""); oss2 << "rm " << sol2->solutionFilename;
	  system(oss2.str().c_str());


	  //*****************************************FILES (REMOVE THIS PART)********************************************************

	  /*fstream fileResults("results", ios::out | ios::app);

	  fileResults << setw(5)  << params.seedValue << setw(30) << params.data_filename
			      << setw(20) << sol->getCost()
		          << setw(20) << sol2->getCost()
		          << setw(20) << *(params.best_objective)
			      << setw(15)  << time(0) - params.programStartTime << endl;

	  // running times
	  fstream fileRunnungTimes("runnung_times", ios::out | ios::app);
	  fileRunnungTimes << setw(5)  << setw(30) << params.data_filename << setw(20) << *(params.best_objective)
	  			       << setw(20)  << time(0) - params.programStartTime << endl;

	  // check solution with official checker
	  ostringstream oss("");
	  oss << "./checker " << params.data_filename << " " << params.initial_assignment_filename
		  << " " << params.solution_filename;
	  oss << " >> checkerFile ";
	  system(oss.str().c_str());
	   */
	  //*************************************************************************************************************************

	  return 0;

}




