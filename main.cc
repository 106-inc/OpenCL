#include "biton.hh"
#include <iostream>

int main(void)
{
  BS::BTS::driver();

  std::vector<int> vec;
  size_t vec_size{};

  // std::cin >> vec_size;

  vec.resize(vec_size);

  for (auto &el : vec)
    std::cin >> el;

  BS::bitonic_sort(vec);

  for (auto el : vec)
    std::cout << el;
  return 0;
}