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
  cl::Device device_{};
  cl::Context contx_{};
  cl::Program prog_{};

  std::string src_code_{};


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

#endif // __BITON_H__