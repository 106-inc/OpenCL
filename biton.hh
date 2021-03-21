#ifndef __BITON_H__
#define __BITON_H__

/* Standard io libraries */
#include <iostream>
#include <fstream>
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
/**
 * @brief OpenCL driver class
 * 
 */
class BTS final
{
private:
  std::vector<cl::Device> devices_{};
  
  std::string src_code{};

public:

  BTS(BTS const &) = delete;
  BTS &operator=(BTS const &) = delete;

  static BTS &driver( void )
  {
    static BTS SingleTone{};

    return SingleTone;
  }

  bool load_src( const std::string &cl_fname )
  {
    std::ifstream src(cl_fname);

    if (!src.is_open())
      return false;

    src_code = {std::istreambuf_iterator<char>(src), std::istreambuf_iterator<char>()};

    return true;
  }

  void prepare_uranus();
private:

  BTS()
  {
    std::vector<cl::Platform> pls;
    std::vector<std::vector<cl::Device>> devs;
    cl::Platform::get(&pls);

    for (auto &&pl_devs : pls)
    {
      std::vector<cl::Device> devs;
      pl_devs.getDevices(CL_DEVICE_TYPE_ALL, &devs);
      for (auto &&dev : devs)
        if (dev.getInfo<CL_DEVICE_COMPILER_AVAILABLE>())
          devices_.push_back(dev);
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