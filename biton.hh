#ifndef __BITON_H__
#define __BITON_H__

/* Standard io libraries */
#include <fstream>
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

// TODO: здесь явно не всё -- разобраться
/**
 * @brief OpenCL driver class
 *
 */

namespace BTS
{

class BSort final
{
private:
  cl::Device device_;
  cl::Context context_;
  cl::CommandQueue queue_;
  cl::Program prog_;

  std::string src_code_;

  bool ready_{false};

  void build();

  enum class Directions
  {
      INCREASING,
      DECREASE,
  };


public:
  BSort(BSort const &) = delete;
  BSort &operator=(BSort const &) = delete;

  static BSort &driver()
  {
    static BSort SingleTone{};

    return SingleTone;
  }

  void operator()(std::vector<int> &vec)
  {
    sort(vec);
  }

  void sort(std::vector<int> &vec);

private:
  BSort();

  bool load_src(const std::string &cl_fname);
};

/**
 * @brief
 *
 * @param data_size
 * @return
 */
bool is_power_2(size_t data_size)
{
  return ((data_size & (data_size - 1)) == 0 && data_size > 1) ? true : false;
}

extern BSort & bsort;

} // namespace BTS
#endif // __BITON_H__