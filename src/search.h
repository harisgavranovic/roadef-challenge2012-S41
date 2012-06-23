#ifndef SEARCH_H_
#define SEARCH_H_

#include "machine.h"
#include "parameters.h"
#include "solution.h"

Solution* local_search_shift_random(Solution* sol, Parameters& params);

Solution* local_search_swap_random (Solution* sol, Parameters& params);


Solution* local_search_shift(Solution* sol, Parameters& params, int max_number_of_moves = 1000000,
							int start_pos = 0, int end_pos = 50000,
		                    int max_number_of_fills = 1000);

Solution* local_search_swap (Solution* sol, Parameters& params,
		                    int max_number_of_moves = 1000000,
							int start_pos = 0, int end_pos = 50000,
		                    int max_number_of_fills = 1000);


void big_process_rearrangement(Solution *sol, Parameters& params, int start_pos = 0, int end_pos = 50000);

#endif /* SEARCH_H_ */
