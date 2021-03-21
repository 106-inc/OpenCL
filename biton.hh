#ifndef __BITON_H__
#define __BITON_H__

/* Standard io library */
#include <iostream>
/*****************************************/

/* std containers */
#include <string>
#include <vector>
/****************************************/

/* OpenCL library */
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
/****************************************/

namespace BS
{
// TODO: здесь явно не всё -- разобраться
class CLDriver final
{
private:
public:
  void find_devices(void)
  {
    std::vector<std::vector<cl::Device>> devs;
    std::vector<cl::Platform> pls;

    cl::Platform::get(&pls);
    devs.resize(pls.size());

    for (size_t i = 0, endi = pls.size(); i < endi; ++i)
      pls[i].getDevices(CL_DEVICE_TYPE_ALL, &devs[i]);

    for (auto &&pl_devs : devs)
    {
      std::cout << "----------------------------\n";
      for (auto &&dev : pl_devs)
        std::cout << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
      std::cout << "----------------------------\n";
    }
    
  }
};

/**
 * @brief Bitonic sort function
 *
 * @param[in] vec reference to vector to sort
 */
void bitonic_sort(std::vector<int> &vec)
{

} /* End of 'bitonic_sort' function */
} // namespace BS

#endif // __BITON_H__