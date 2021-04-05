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

    Time::Timer timer; 
    std::sort(comp_vec.begin(), comp_vec.end());
    std::cout << "std::sort(...) time: " << timer.elapsed() << " microsecons\n";

    BTS::bsort(vec, BTS::Dir::INCR);
    
#if 0
    for (size_t i = 0; i < vec_size; ++i)
        std::cout << vec[i] << " ";

#endif 

    std::cout << std::endl;

    if (vec == comp_vec)
    {
        std::cout << "Sorting was succesfully!\n";
        return 0;
    }
    else 
    {
        std::cout << "Sorting wasn't succesfully!\n";
        return -1;
    }

    std::cout << std::endl;
}