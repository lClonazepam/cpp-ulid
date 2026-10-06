#include "ulid.hpp"
#include <cassert>
#include <iostream>
#include <set>

int main() {
  std::set<std::string> ids;
  std::string prev;
  for (int i = 0; i < 50; ++i) {
    auto id = kit::Ulid().str();
    assert(id.size() == 26);
    assert(ids.insert(id).second);
    if (!prev.empty()) assert(prev <= id);
    prev = id;
    assert(kit::Ulid::parse(id).str() == id);
  }
  std::cout << "ulid ok " << prev << "\n";
}
