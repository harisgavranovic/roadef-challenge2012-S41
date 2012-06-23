#include "IO.h"


Data* readDataFromFilesAndConstructSolution(
		string data_filename,
		string initial_solution_filename,
		string new_solution_filename, Solution* sol)
{

    Data* data = new Data();

	fstream dataFile(data_filename.c_str(), ios_base::in);

	int x;

	// parse resources
	int number_of_resources;
    dataFile >> number_of_resources;
	for(int i = 0; i < number_of_resources; i++)
    {
	   bool is_transient = false;
	   int weight;
       dataFile >> x >> weight;
       if(x == 1) is_transient = true;
       Resource* resource = new Resource(i, is_transient, 10 * weight);
       data->addResource(resource);
    }



	// parse machines
	int number_of_machines;
	vector<bool> neighborhood_used(10000, false);
	vector<bool> location_used(10000, false);
    dataFile >> number_of_machines;
	for(int i = 0; i < number_of_machines; i++)
    {

	   int hood, loc;
       dataFile >> hood >> loc;

       //add new neighborhood
       if(!neighborhood_used[hood])
       {
          data->addNeighborhood(new Neighborhood(hood));
          neighborhood_used[hood] = true;
       }

       //add new location
       if(!location_used[loc])
       {
          data->addLocation(new Location(loc));
          location_used[loc] = true;
       }


       Machine* machine = new Machine(i, data);
       machine->setNeighborhood(data->getNeighborhood(hood));
       machine->setLocation(data->getLocation(loc));


       // capacities
       for(int r = 0; r < number_of_resources; r++)
       {
    	   int64 cap;
    	   dataFile >> cap;
    	   MachineResource* m_r = new MachineResource(machine, data->getResource(r));
    	   m_r->setCapacity(cap);
    	   machine->addMachineResource(m_r);
       }

       // safety capacities
       for(int r = 0; r < number_of_resources; r++)
       {
     	   int64 s_cap;
     	   dataFile >> s_cap;
     	   machine->getMachineResource(r)->setSafetyCapacity(s_cap);
       }
       // mmc
       for(int m = 0; m < number_of_machines; m++)
       {
     	   int64 mmc;
     	   dataFile >> mmc;
     	   machine->addToMachineMoveCost(mmc);
       }

       data->addMachine(machine);

    }



	// parse services
	int number_of_services;
    dataFile >> number_of_services;
    //cout << number_of_services << endl;
    vector<vector<int> > dependencies(number_of_services, vector<int>(0));
	for(int i = 0; i < number_of_services; i++)
    {

	   int spread_min, number_of_dependencies;
       dataFile >> spread_min >> number_of_dependencies;

       Service* service = new Service(i, spread_min, data);

       data->addService(service);

       for(int d = 0; d < number_of_dependencies; d++)
       {
           dataFile >> x;
    	   dependencies[i].push_back(x);
       }
    }

	for(int i = 0; i < number_of_services; i++)
	   data->getService(i)->InitializeVectors(data);


	for(int i = 0; i < number_of_services; i++)
       for(int d = 0; d < dependencies[i].size(); d++)
    	   data->getService(i)->addDependency(data->getService(dependencies[i][d]));

	// parse processes
	int number_of_processes;
    dataFile >> number_of_processes;
	for(int i = 0; i < number_of_processes; i++)
    {

	   int service_id ;
       dataFile >> service_id;
       int64 move_cost;
       vector<int64> requirements (number_of_resources, 0);
       for(int r = 0; r < number_of_resources; r++)
         dataFile >> requirements[r];
       dataFile >> move_cost;

       Process* process = new Process(i, move_cost, data->getService(service_id));

       for(int r = 0; r < number_of_resources; r++)
    	  process->addRequirement(requirements[r]);

       data->addProcess(process);

    }


	// parse balances
	int number_of_balances;
    dataFile >> number_of_balances;
	for(int i = 0; i < number_of_balances; i++)
    {
	   int r_1, r_2, target, weight ;
       dataFile >> r_1 >> r_2 >> target >> weight;
       Balance* balance = new Balance(r_1, r_2, target, 10 * weight);
       data->addBalance(balance);
    }

	// parse weights
    int64 pmcw, smcw, mmcw;
	dataFile >> pmcw >> smcw >> mmcw;
    data->setProcessMoveCostWeight(10 * pmcw);
    data->setServiceMoveCostWeight(10 * smcw);
    data->setMachineMoveCostWeight(10 * mmcw);


    sol->setData(data);

    // parse initial solution
    for(int i = 0; i < data->getNumberOfProcesses(); i++) sol->addProcess(data->getProcess(i));
    for(int i = 0; i < data->getNumberOfMachines(); i++)  sol->addMachine(data->getMachine(i));
    for(int i = 0; i < data->getNumberOfServices(); i++)  sol->addService(data->getService(i));

    fstream initialSolutionFile(initial_solution_filename.c_str(), ios_base::in);

    for(int i = 0; i < data->getNumberOfProcesses(); i++)
    {
    	initialSolutionFile >> x;
    	sol->assignProcessInitial(sol->getProcess(i), data->getMachine(x));
	}


    sol->fillMatrix();

    fstream newSolutionFile(new_solution_filename.c_str(), ios_base::in);

    for(int i = 0; i < data->getNumberOfProcesses(); i++)
    {
    	newSolutionFile >> x;
    	sol->reassignProcess(sol->getProcess(i), data->getMachine(x));
	}

    //calculate total requirements and total capacity for each resource
    for(int r = 0; r < data->getNumberOfResources(); r++)
    {
       int64 tot_cap(0), tot_safety(0), tot_req(0);

 	   for(int p = 0; p < data->getNumberOfProcesses(); p++)
 		   tot_req += data->getProcess(p)->getRequirement(r);

 	   for(int m = 0; m < data->getNumberOfMachines(); m++) {
 		   tot_cap    += data->getMachine(m)->getMachineResource(r)->getCapacity();
 		   tot_safety += data->getMachine(m)->getMachineResource(r)->getSafetyCapacity();

 	   }

 	   data->getResource(r)->setTotalCapacity(tot_cap);
 	   data->getResource(r)->setTotalRequirement(tot_req);
 	   data->getResource(r)->setTotalSafetyCapacity(tot_safety);

    }


    data->calculateProcessesPositionsByRequirements();
    sol->sortProcessesByPosition();

    return data;
}
