#ifndef TESTOVI_H_
#define TESTOVI_H_


#include <cmath>
#include <map>
using namespace std;

#include "IO.h"
#include "machine.h"
#include "parameters.h"
#include "search.h"


//read data from files and construct solution (initial or start solution)
Solution* constructSolution(Parameters& params);

Solution* constructSolutionForMultipliedInstance(Parameters& params, int number_of_multiplies);


void readSolutionFromFile(Solution* sol, string filename);


// big process rearrangement
// shift and improve capacity
void BPR(Solution* sol, Parameters& params, int iterations = 100, int start_pos = 0, int end_pos = 50000);


// decrease machine capacities and set process requirements
// to zero if process can be assigned to one machine only
Data* preprocesData(Solution* sol);

int64 LB(Solution* sol);



int64 solveAFinal(Parameters& params, Solution* startSol = 0);
int64 solveBFinal(Parameters& params, Solution* startSol = 0);
int64 solveBFinal2(Parameters& params, Solution* startSol = 0);

// boost
long SolveNegativeEdgesCyclePlusClosedCyclesPlusChainRangeRequierements(Solution* best_sol, Parameters& params,
		int nmb_iter, string constraints_string,
		std::fstream& file_stat, int lowerRange, int upperRange, int nmb_vertices = 0);

void solveBPRplusBoost(Parameters& params, Solution* startSol = 0);


#endif /* TESTOVI_H_ */
