// Wrapper peste un set PBDS, pentru benchmarking.
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <string>

typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less<int>,
  __gnu_pbds::rb_tree_tag,
  __gnu_pbds::tree_order_statistics_node_update
  > ordered_set;

struct data_structure {
  ordered_set s;

  const std::string get_description() {
    return "set PBDS";
  }

  void init() {
    s.clear();
  }

  void insert(int key) {
    s.insert(key);
  }

  bool contains(int key) {
    return s.find(key) != s.end();
  }

  int order_of(int key) {
    return s.order_of_key(key);
  }

  int kth_element(int k) {
    return *s.find_by_order(k);
  }

  void erase(int key) {
    s.erase(key);
  }
};
