#ifndef RESOURCE_H_
#define RESOURCE_H_

struct Resource {
  Resource(int id, bool _is_transient, int _load_cost_weight)
      : id_(id),
        is_transient(_is_transient),
        load_cost_weight(_load_cost_weight),
        original_load_cost_weight(_load_cost_weight) {}

  int id_;
  bool is_transient;
  int load_cost_weight;
  int original_load_cost_weight;

  int64 total_capacity;
  int64 total_safety_capacity;
  int64 total_requirement;

  void setTotalCapacity(int64 x) {total_capacity = x;}
  void setTotalSafetyCapacity(int64 x) {total_safety_capacity = x;}
  void setTotalRequirement(int64 x) {total_requirement = x;}

};



#endif /* RESOURCE_H_ */
