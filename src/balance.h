#ifndef BALANCE_H_
#define BALANCE_H_

struct Balance {


  int first_resource_id;
  int second_resource_id;
  int target;
  int weight;


	Balance(int _first_resource_id, int _second_resource_id,
            int _target, int _weight)
    : first_resource_id(_first_resource_id),
      second_resource_id(_second_resource_id),
      target(_target),
      weight(_weight) {}

};

#endif /* BALANCE_H_ */
