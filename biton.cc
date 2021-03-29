#include "biton.hh"

namespace BTS
{

void bsort(cl::vector<int> &vec, Dir dir /* = Dir::INCR */)
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

  try
  {
    prog_.build();
  }
  catch (const cl::Error &build_err)
  {
    std::cerr << "Error in " << build_err.what() << std::endl;
    std::cerr << err_what(build_err.err()) << std::endl;
    std::cerr << prog_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_);
    ready_ = false;
  }
}

/**
 * @brief bitonic sort array fucntion
 *
 * @param[in, out] vec vector to sort
 */
void BSort::sort(cl::vector<int> &vec, Dir dir)
{
  if (!ready_)
  {
    std::cerr << "Errors occured" << std::endl;
    return;
  }

  size_t data_size = vec.size();

  bool res = is_power_2(data_size);

  if (res)
    sort_extended(vec, Dir::INCR);

  // here goes a program
} /* End of 'sort' function */

void BSort::sort_extended(cl::vector<int> &vec, Dir dir)
{
  size_t data_size = vec.size(); /*, num_of_pairs = log2(data_size); i really dont know */
  size_t num_of_pairs = std::log2(data_size);

  //* this var will be usefull in local_bitonic, global_btionic
  cl::NDRange glob_size(data_size);
  cl::NDRange loc_size(1);
  cl::NDRange offset(0);
  //* but now it useless

  cl::Buffer buf(context_, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE, data_size * sizeof(int), vec.data());

  //! Loop on sorted sequence length
  for (size_t cur_pair = 0; cur_pair < num_of_pairs; ++cur_pair)
  {
    //! Loop on comparison distance (between elems)
    for (size_t dist_pair = cur_pair; dist_pair > 0; dist_pair >>= 1)
    {
      try
      {
        cl::Kernel kernel(prog_, "bitonic_sort");

        kernel.setArg(0, buf);
        kernel.setArg(1, static_cast<unsigned>(cur_pair));
        kernel.setArg(2, static_cast<unsigned>(dist_pair));
        kernel.setArg(3, static_cast<unsigned>(dir));

        kernel_exec(kernel, offset, glob_size, loc_size);
      }
      catch (cl::Error &err)
      {
        std::cerr << "Error occured in " << err.what() << std::endl;
        std::cerr << err_what(err.err()) << std::endl;

        return;
      }
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

const char *err_what(cl_int err_code)
{
  switch (err_code)
  {
  case 0:
    return "CL_SUCCESS";
  case -1:
    return "CL_DEVICE_NOT_FOUND";
  case -2:
    return "CL_DEVICE_NOT_AVAILABLE";
  case -3:
    return "CL_COMPILER_NOT_AVAILABLE";
  case -4:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case -5:
    return "CL_OUT_OF_RESOURCES";
  case -6:
    return "CL_OUT_OF_HOST_MEMORY";
  case -7:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case -8:
    return "CL_MEM_COPY_OVERLAP";
  case -9:
    return "CL_IMAGE_FORMAT_MISMATCH";
  case -10:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case -11:
    return "CL_BUILD_PROGRAM_FAILURE";
  case -12:
    return "CL_MAP_FAILURE";
  case -13:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
  case -14:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
  case -15:
    return "CL_COMPILE_PROGRAM_FAILURE";
  case -16:
    return "CL_LINKER_NOT_AVAILABLE";
  case -17:
    return "CL_LINK_PROGRAM_FAILURE";
  case -18:
    return "CL_DEVICE_PARTITION_FAILED";
  case -19:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
  case -30:
    return "CL_INVALID_VALUE";
  case -31:
    return "CL_INVALID_DEVICE_TYPE";
  case -32:
    return "CL_INVALID_PLATFORM";
  case -33:
    return "CL_INVALID_DEVICE";
  case -34:
    return "CL_INVALID_CONTEXT";
  case -35:
    return "CL_INVALID_QUEUE_PROPERTIES";
  case -36:
    return "CL_INVALID_COMMAND_QUEUE";
  case -37:
    return "CL_INVALID_HOST_PTR";
  case -38:
    return "CL_INVALID_MEM_OBJECT";
  case -39:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case -40:
    return "CL_INVALID_IMAGE_SIZE";
  case -41:
    return "CL_INVALID_SAMPLER";
  case -42:
    return "CL_INVALID_BINARY";
  case -43:
    return "CL_INVALID_BUILD_OPTIONS";
  case -44:
    return "CL_INVALID_PROGRAM";
  case -45:
    return "CL_INVALID_PROGRAM_EXECUTABLE";
  case -46:
    return "CL_INVALID_KERNEL_NAME";
  case -47:
    return "CL_INVALID_KERNEL_DEFINITION";
  case -48:
    return "CL_INVALID_KERNEL";
  case -49:
    return "CL_INVALID_ARG_INDEX";
  case -50:
    return "CL_INVALID_ARG_VALUE";
  case -51:
    return "CL_INVALID_ARG_SIZE";
  case -52:
    return "CL_INVALID_KERNEL_ARGS";
  case -53:
    return "CL_INVALID_WORK_DIMENSION";
  case -54:
    return "CL_INVALID_WORK_GROUP_SIZE";
  case -55:
    return "CL_INVALID_WORK_ITEM_SIZE";
  case -56:
    return "CL_INVALID_GLOBAL_OFFSET";
  case -57:
    return "CL_INVALID_EVENT_WAIT_LIST";
  case -58:
    return "CL_INVALID_EVENT";
  case -59:
    return "CL_INVALID_OPERATION";
  case -60:
    return "CL_INVALID_GL_OBJECT";
  case -61:
    return "CL_INVALID_BUFFER_SIZE";
  case -62:
    return "CL_INVALID_MIP_LEVEL";
  case -63:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
  case -64:
    return "CL_INVALID_PROPERTY";
  case -65:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
  case -66:
    return "CL_INVALID_COMPILER_OPTIONS";
  case -67:
    return "CL_INVALID_LINKER_OPTIONS";
  case -68:
    return "CL_INVALID_DEVICE_PARTITION_COUNT";
  case -69:
    return "CL_INVALID_PIPE_SIZE";
  case -70:
    return "CL_INVALID_DEVICE_QUEUE";
  case -71:
    return "CL_INVALID_SPEC_ID";
  case -72:
    return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";

  default:
    return "UNCKNOWN ERROR";
  }
}

} // namespace BTS