#include <biton.hh>

/**
 * @brief Construct a new BTS::BTS object function
 *
 */
BTS::BTS(void)
{
  std::vector<cl::Platform> pls;
  cl::Platform::get(&pls);

  for (auto &&pl_devs : pls)
  {
    std::vector<cl::Device> devs;
    pl_devs.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    for (auto &&dev : devs)
      if (dev.getInfo<CL_DEVICE_COMPILER_AVAILABLE>())
      {
        device_ = dev;
        break;
      }
  }

  /*ВОЗОЖНО ПОНАДОБИТСЯ БРОСАТЬ ИСКЛЮЧЕНИЯ И ВСЕ ЭТО ДЕЛО РАЗДЕЛЯТЬ*/

  context_ = cl::Context{device_};
} /* End of 'BTS' function */



/**
 * @brief bitonic sort array fucntion
 *
 * @param[in, out] vec vector to sort
 */
void BTS::sort(std::vector<int> &vec)
{
  //cl::Context cont{device_};
  cl::CommandQueue queue{context_, device_};

  cl::Buffer buf{vec.begin(), vec.end(), true};

  cl::Program::Sources sources{1, std::make_pair(src_code_.c_str(), src_code_.size())};
  //cl::Program prog{cont, sources};
  prog_{context_, sources};

  prog.build({device_});

  size_t data_size = vec.size();

  bool res = is_power_2(data_size);

  if (res)
      sort_extended(vec, Directions::INCREASING);




  //cl::Kernel kern{prog, "Bitonic sort"};


  // here goes a program
} /* End of 'sort' function */

/**
 * @brief Load .cl source code from file
 *
 * @param[in] cl_fname name of a file with .cl code
 * @return true if all is ok
 * @return false otherwise
 */
bool BTS::load_src(const std::string &cl_fname)
{
  std::ifstream src(cl_fname);

  if (!src.is_open())
    return false;

  src_code_ = {std::istreambuf_iterator<char>(src), std::istreambuf_iterator<char>()};

  return true;
} /* Edn of 'load_src' function */