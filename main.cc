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

    BTS::bsort(vec, BTS::Dir::INCR);


    for (size_t i = 0; i < vec_size; ++i)
        std::cout << vec[i] << " ";

    std::cout << std::endl;
}