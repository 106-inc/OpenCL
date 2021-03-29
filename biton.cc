#include "biton.hh"

namespace BTS
{

void bsort(std::vector<int> &vec, Dir dir /* = Dir::INCR */)
{
  BSort::driver().sort(vec, dir);
}

/**
 * @brief Construct a new BSort::BSort object function
 *
 */
BSort::BSort()
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
} /* End of 'BSort' function */

void BSort::build()
{
  load_src("biton.cl");

  cl::Program::Sources sources{1, std::make_pair(src_code_.c_str(), src_code_.size())};
  prog_ = cl::Program{context_, sources};

  try
  {
    prog_.build();
  }
  catch (const cl::Error &build_err)
  {
    std::cerr << build_err.what() << std::endl;
    ready_ = false;
  }
}

/**
 * @brief bitonic sort array fucntion
 *
 * @param[in, out] vec vector to sort
 */
void BSort::sort(std::vector<int> &vec, Dir dir)
{
  cl::Buffer buf{vec.begin(), vec.end(), true};

  cl::Kernel kern{prog_, "Bitonic sort"};
  size_t data_size = vec.size();

  bool res = is_power_2(data_size);

  if (res)
    sort_extended(vec, Dir::INCR);

  // here goes a program
} /* End of 'sort' function */

void BSort::sort_extended(std::vector<int> &vec, Dir dir)
{
  size_t data_size = vec.size(), num_of_pairs = log2(data_size);

  cl::NDRange glob_size{num_of_pairs};
  cl::NDRange loc_size{};

  cl::Kernel kernel(prog_, "biton_sort");

  cl::Buffer buf(context_, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data_size * sizeof(int), vec.data());

  /*
  for (size_t cur_pair_left = 0; cur_pair < num_of_pairs; ++cur_pair)
  {
      for (size_t cur_pair_rht; cur_pair_2 < cur_pair + num_of_pair; ++
  }*/
}

/**
 * @brief enqueues a command to execute a kernel on a device.
 * @param kernel
 * @param offset
 * @param glob_size
 * @param loc_size
 */
bool BSort::kernel_exec(const cl::Kernel &kernel, const cl::NDRange &offset, const cl::NDRange &glob_size,
                        const cl::NDRange &loc_size)
{
  cl::Event event;
  if (queue_.enqueueNDRangeKernel(kernel, offset, glob_size, loc_size, &event) != cl::CL_SUCCESS)
    //! maybe exception
    return false;
}

/**
 * @brief Load .cl source code from file
 *
 * @param[in] cl_fname name of a file with .cl code
 * @return true if all is ok
 * @return false otherwise
 */
bool BSort::load_src(const std::string &cl_fname)
{
  std::ifstream src(cl_fname);

  if (!src.is_open())
    return false;

  src_code_ = {std::istreambuf_iterator<char>(src), std::istreambuf_iterator<char>()};

  return true;
} /* Edn of 'load_src' function */

} // namespace BTS