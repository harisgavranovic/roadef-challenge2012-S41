#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// Required by for routine
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>

#include <stdlib.h>   // eclaration for exit()
#include <stdio.h>   // eclaration

#include <string.h>
using namespace std;


struct Parameters{


	int ID;

	string data_filename;
	string solution_filename;
	string initial_assignment_filename;
	string team_name;
	long long seedValue;
	string new_assignment_filename;
	int time_limit;

	int nmb_of_iterations_in_local_search;
	int nmb_repeats;

	fstream* fileFinal;

	int64 programStartTime;

	int    numberOfBestProcessesToConsiderShiftSwap;
	int position_type;

	int delta ;
    int nmb_iters_bpr;
    int nmbRanges;
    int rangeLength;
    int nmbLoops;
    int multiply_factor;


	int64* best_objective;

    Parameters() {}

	Parameters(int argc, char **argv, int drugi = 0)
	{


		ID = drugi;

	    programStartTime = time(0);

		team_name= "S41";
		seedValue = time(0);

		numberOfBestProcessesToConsiderShiftSwap = 50000;

		delta = 40;
	    nmb_iters_bpr = 300;
	    nmbRanges = 7;
	    rangeLength = 1000 / 7;

	    multiply_factor = 1;
	    nmbLoops = 1;

	    nmb_of_iterations_in_local_search = 700000;
	    nmb_repeats = 1000;

		for(int param = 1; param < argc; param++)
		{

			if(string(argv[param]) == string("-p"))
				data_filename = string(argv[param+1]);
			else
			if(string(argv[param]) == string("-o"))
				solution_filename = string(argv[param+1]);
			else
			if(string(argv[param]) == string("-i"))
				initial_assignment_filename = string(argv[param+1]);
			else
			if(string(argv[param]) == string("-name")) {
				cout << "S41" << endl;
				if (argc == 2) exit(0);
			}
			else
			if(string(argv[param]) == string("-s")) {
				seedValue = atoi(argv[param+1]);
			}
			else
			if(string(argv[param]) == string("-n"))
				new_assignment_filename = string(argv[param+1]);
			else
			if(string(argv[param]) == string("-t"))
				time_limit = atoi(argv[param+1]);
	  }


	  if(new_assignment_filename == "") new_assignment_filename = initial_assignment_filename;

	  ostringstream oss("");
	  if (drugi == 0)
  	  {
		oss << "statistics/" << data_filename.substr(data_filename.length() - 8, 4) << "0Final";
		fileFinal = new fstream(oss.str().c_str(), ios::out | ios::app);
	  }
	  else
  	  {
		oss << "statistics/" << data_filename.substr(data_filename.length() - 8, 4) << "0Final" << drugi;
		fileFinal = new fstream(oss.str().c_str(), ios::out | ios::app);
	  }

  }

};



#endif /* PARAMETERS_H_ */
