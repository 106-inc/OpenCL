#ifndef __BITON_H__
#define __BITON_H__

/* Standard io libraries */
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

/*****************************************/

/* std containers */
#include <string>
#include <vector>
/****************************************/

/* OpenCL library */
#include <CL/cl2.hpp>

/****************************************/

/* Our libs */

#include "timer.hh"

/****************************************/

// TODO: здесь явно не всё -- разобраться

/**
 * @brief OpenCL driver class
 *
 */
namespace BTS
{
enum class Dir
{
  DECR,
  INCR
};

class BSort
{
private:
  cl::Context context_;
  cl::Device device_;

  cl::Program::Sources sources_;
  cl::CommandQueue queue_;
  cl::Program prog_;

  cl::Kernel simple_sort_;
  cl::Kernel fast_sort_;

  std::string src_code_;

  size_t work_group_size = 0;

private:
  BSort();

  bool build();

  void Vec_preparing(std::vector<int> &vec, Dir dir);

  bool kernel_exec(cl::Kernel kernel, size_t global_size, size_t local_size, std::vector<cl::Event> &events);

  void gpu_timing(std::vector<cl::Event> &events, cl_ulong *time);

public:
  BSort(BSort const &) = delete;
  BSort &operator=(BSort const &) = delete;

  static BSort &driver()
  {
    static BSort SingleTone{};
    return SingleTone;
  }

  void sort_extended(std::vector<int> &vec, Dir dir = Dir::INCR);

  void Device_selection(); // choose first suited platform and device
};

void bsort(std::vector<int> &vec, Dir dir);
const char *err_what(cl_int err_code);
} // namespace BTS

#endif