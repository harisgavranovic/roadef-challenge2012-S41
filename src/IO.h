#ifndef IO_H_
#define IO_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;

#include "machine.h"
#include "types.h"
#include "resource.h"
#include "solution.h"
#include "service.h"

// reads data from "data_filename"
// constructs initial solution and saves it to "sol"
// returns Data
Data* readDataFromFilesAndConstructSolution(
		string data_filename,
		string initial_solution_filename,
		string new_solution_filename, Solution* sol);

#endif /* IO_H_ */
