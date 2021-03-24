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



class BTS final
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
  BTS(BTS const &) = delete;
  BTS &operator=(BTS const &) = delete;

  static BTS &driver(void)
  {
    static BTS SingleTone{};

    return SingleTone;
  }

  void operator()(std::vector<int> &vec)
  {
    sort(vec);
  }

  void sort(std::vector<int> &vec);

private:
  BTS(void);

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
    if ((data_size & (data_size - 1)) == 0 && data_size > 1)
        return true;
    else
        return false;
}

#endif // __BITON_H__