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
    std::vector<cl::Device> devs;
    cl::Platform pl;
    pl.getDevices(CL_DEVICE_TYPE_ALL, &devs);

    for (auto &&dev : devs)
    {
      std::string name;
      dev.getInfo(CL_DEVICE_NAME, &name);
      std::cout << name << std::endl;
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