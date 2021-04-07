#include "biton.hh"

int main()
{
  std::vector<int> vec;
  size_t vec_size{};

  std::cin >> vec_size;

  if (!std::cin.good())
  {
    std::cout << "Invalid character at stdin" << std::endl;
    return -1;
  }

  vec.resize(vec_size);

  for (auto &el : vec)
  {
    std::cin >> el;
    if (!std::cin.good())
    {
      std::cout << "Invalid character at stdin" << std::endl;
      return -1;
    }
  }

  std::vector<int> comp_vec{vec};

#if 0
    Time::Timer timer; 
    std::sort(comp_vec.begin(), comp_vec.end());
    std::cout << "std::sort(...) time: " << timer.elapsed() << " microseconds\n";
#endif

  try
  {
    BTS::bsort(vec, BTS::Dir::INCR);

    for (size_t i = 0; i < vec_size; ++i)
      std::cout << vec[i] << " ";

    std::cout << std::endl;
  }
  catch (std::exception &err)
  {
    std::cerr << "Error occured in " << err.what() << std::endl;
    return -1;
  }

#if 0
    std::cout << std::endl;

    if (vec == comp_vec)
    {
        std::cout << "Sortion was succesful!\n";
        return 0;
    }
    else 
    {
        std::cout << "Sortion wasn't succesful!\n";
        return -1;
    }
#endif
}