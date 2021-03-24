#include <biton.hh>

/**
 * @brief Construct a new BTS::BTS object function
 *
 */
BTS::BTS()
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
        ready_ = true;
        break;
      }
  }

  if (!ready_)
    return;

  context_ = cl::Context{device_};
  build();
  queue_ = cl::CommandQueue{context_, device_};
} /* End of 'BTS' function */

void BTS::build()
{

}

/**
 * @brief bitonic sort array fucntion
 *
 * @param[in, out] vec vector to sort
 */
void BTS::sort(std::vector<int> &vec)
{
  cl::Buffer buf{vec.begin(), vec.end(), true};

  cl::Program::Sources sources{1, std::make_pair(src_code_.c_str(), src_code_.size())};
  //cl::Program prog{cont, sources};
  prog_ = cl::Program{context_, sources};

  prog_.build({device_});

  cl::Kernel kern{prog_, "Bitonic sort"};
  size_t data_size = vec.size();

  bool res = is_power_2(data_size);

  if (res)
      sort_extended(vec, Directions::INCREASING);


  // here goes a program
} /* End of 'sort' function */


void BTS::sort_extended(std::vector<int> &vec)
{
    size_t data_size = vec.size(), num_of_pairs = 0, start_size = 1;


    while (start_size != data_size)
    {
        start_size *= 2;
        ++num_of_pairs;
    }

    cl::Buffer buf(context_, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                             data_size * sizeof(int), vec.data());


}


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