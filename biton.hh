#ifndef __BITON_H__
#define __BITON_H__

/* Standard io libraries */
#include <cmath>
#include <fstream>
#include <iostream>
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
  INCR,
  DECR
};

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

public:
  BSort(BSort const &) = delete;
  BSort &operator=(BSort const &) = delete;

  static BSort &driver()
  {
    static BSort SingleTone{};

    return SingleTone;
  }

  void sort(std::vector<int> &vec, Dir dir);

private:
  BSort();

  bool load_src(const std::string &cl_fname);

  void sort_extended(std::vector<int> &vec, Dir dir);

  bool kernel_exec(const cl::Kernel &kernel, const cl::NDRange &offset, const cl::NDRange &global,
                   const cl::NDRange &local);
};

bool is_power_2(size_t data_size);

void bsort(std::vector<int> &vec, Dir dir = Dir::INCR);

} // namespace BTS
#endif // __BITON_H__