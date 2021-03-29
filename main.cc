#include "biton.hh"

int main()
{
  std::vector<int> vec;
  size_t vec_size{};

  std::cin >> vec_size;

  vec.resize(vec_size);

  for (auto &el : vec)
    std::cin >> el;

  BTS::bsort(vec);

  for (auto el : vec)
    std::cout << el << std::endl;

  return 0;
}