#include "biton.hh"

namespace BTS
{

void bsort(std::vector<int> &vec, Dir dir /* = Dir::INCR */)
{
  BSort::driver().sort(vec, dir);
}

/**
 * @brief Check if value is a power of 2
 *
 * @param[in] data_size value to check
 * @return true if value is a power of 2, false otherwise
 */
bool is_power_2(size_t data_size)
{
  return ((data_size & (data_size - 1)) == 0 && data_size > 1) ? true : false;
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
    pl_devs.getDevices(CL_DEVICE_TYPE_GPU, &devs);
    for (auto &&dev : devs)
      if (dev.getInfo<CL_DEVICE_AVAILABLE>() && dev.getInfo<CL_DEVICE_COMPILER_AVAILABLE>())
      {
        device_ = dev;
        ready_ = true;
        break;
      }
    if (ready_)
      break;
  }

  if (!ready_)
    return;

  context_ = cl::Context{device_};
  build();
  queue_ = cl::CommandQueue{context_, device_, CL_QUEUE_PROFILING_ENABLE};
} /* End of 'BSort' function */

void BSort::build()
{
  load_src("biton.cl");

  cl::Program::Sources sources{src_code_.c_str()};
  prog_ = cl::Program{context_, sources};

  std::cout << device_.getInfo<CL_DEVICE_NAME>();
  
  try
  {
    prog_.build({device_});
  }
  catch (const cl::Error &build_err)
  {
    std::cerr << "Error in " << build_err.what() << std::endl;
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
  if (!ready_)
  {
    std::cerr << "Errors occured" << std::endl;
    return;
  }

  cl::Buffer buf{vec.begin(), vec.end(), false};

  cl::Kernel kern{prog_, "Bitonic sort"};
  size_t data_size = vec.size();

  bool res = is_power_2(data_size);


  if (res)
    sort_extended(vec, Dir::INCR);

  // here goes a program
} /* End of 'sort' function */

void BSort::sort_extended(std::vector<int> &vec, Dir dir)
{
  size_t data_size = vec.size();               /*, num_of_pairs = log2(data_size); i really dont know */
  size_t num_of_pairs = 0, tmp_data_size = 1;  /* what is the best idea : cycle or log2               */

  while (tmp_data_size < data_size)
  {
      tmp_data_size *= 2;
      ++num_of_pairs;
  }

  //* this var will be usefull in local_bitonic, global_btionic
  cl::NDRange glob_size(data_size);
  cl::NDRange loc_size(1);
  cl::NDRange offset(0);
  //* but now it useless

  cl::Buffer buf(context_, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data_size * sizeof(int), vec.data());

  //! Loop on sorted sequence length
  for (size_t cur_pair = 0; cur_pair < num_of_pairs; ++cur_pair)
  {
      //! Loop on comparison distance (between elems)
      for (size_t dist_pair = cur_pair; dist_pair > 0; dist_pair >>= 1)
      {
          cl::Kernel kernel(prog_, "biton_sort");
          kernel.setArg(0, vec);
          kernel.setArg(1, cur_pair);
          kernel.setArg(2, dist_pair);
          kernel.setArg(3, dir);

          kernel_exec(kernel, offset, glob_size, loc_size);
      }
  }

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

#if 0 
  cl::Event event;
#endif

  int err_num = queue_.enqueueNDRangeKernel(kernel, offset, glob_size, loc_size, NULL, NULL);
  
  if (err_num != CL_SUCCESS)
    return false;

  return true;
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