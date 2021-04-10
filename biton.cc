#include "biton.hh"

namespace BTS
{

/**
 * @brief
 *
 * @param vec
 * @param dir
 */
void bsort(std::vector<int> &vec, Dir dir)
{
  BSort::driver().sort_extended(vec, dir);
  /*
  BSort ctor{};
  ctor.sort_extended(vec, dir);*/
} /* End of 'bsort' function */

/**
 * @brief Construct a new BSort::BSort object function
 *
 */
BSort::BSort()
{
  Device_selection();

  //! Getting the size of the ND range space that can be handled by a single invocation of a kernel compute unit. 
  work_group_size = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

  context_ = cl::Context{device_};
  queue_ = cl::CommandQueue{context_, device_, CL_QUEUE_PROFILING_ENABLE};

  if (!build())
    throw std::runtime_error{"Building of program wasn't sucsessful!\n"};
} /* End of 'BSort' function */

/**
 * @brief Function for selecting gpu-device
 */
void BSort::Device_selection()
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
        return;
      }
  }

  throw std::runtime_error("Devices didn't find!\n");
} /* End of 'Device_selection' function */

/**
 * @brief build - helper function for constructor: creating any members of class
 *
 * @return true
 * @return false
 */
bool BSort::build()
{
  src_code_ = {
    #include "biton.cl"
  };

  sources_ = cl::Program::Sources{src_code_};

  prog_ = cl::Program(context_, sources_);

  try
  {
    prog_.build();
  }
  catch(cl::Error &error)
  {
      std::cerr << error.what() << std::endl;
      std::cerr << prog_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_) << std::endl;
  }

  simple_sort_ = cl::Kernel(prog_, "simple_sort");
  fast_sort_ = cl::Kernel(prog_, "fast_sort");

  return true;
} /* End of 'build' function */



/**
 * @brief sort_extended - sort, which called by user in main
 *        "extened" because it work not only with numbers is a power of two
 *
 * @param vec
 * @param dir
 */
void BSort::sort_extended(std::vector<int> &vec, Dir dir) 
{       
    //! Getting old size for resizing in future out vec
    size_t old_vec_size = vec.size();

    //! Resizing our vec for working with a number that is a power of two
    Vec_preparing(vec, dir);
    size_t new_vec_size = vec.size();

    //! global_size <=> number of work-items that I wish to execute
    //! vec.size() / 2 cause work item shall compare two elems
    size_t glob_size = vec.size() / 2;

    //! local_size <=> number of work-items that I wish to group into a work-group
    //! size of loc_size should be less or equal work_group
    //! This is the reason of comparing elems on this distance
    size_t loc_size = std::min(glob_size, work_group_size);

    //! Creating special buffer for working with glod memory in kernel
    cl::Buffer buffer(context_, CL_MEM_READ_WRITE, sizeof(int) * vec.size());
    queue_.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(int) * vec.size(), vec.data());

    //! Getting number of pairs of our resized vector
    uint num_of_pairs = std::ceil(std::log2(new_vec_size));

    uint cur_stage = std::log2(loc_size);

    //! Allocation local memory for working in fast_sort_
    cl::LocalSpaceArg local = cl::Local(2 * loc_size * sizeof(int));

    cl::Event event; 

#if TIME
    Time::Timer timer_gpu;
#endif

    //! Setting args for execution fast_sort_
    try
    {
        fast_sort_.setArg(0, buffer);
        fast_sort_.setArg(1, cur_stage);
        fast_sort_.setArg(2, local);
        fast_sort_.setArg(3, static_cast<unsigned>(dir));

        //! fast_sort_ execution
        if (!kernel_exec(fast_sort_, glob_size, loc_size, event))
            throw std::runtime_error{"Execution of simple_sort wasn't sucsessful!\n"};
    }
    catch (cl::Error& err)
    {
        std::cerr << "Error occured in " << err.what() << std::endl;
        std::cerr << err_what(err.err()) << std::endl;
    }

    /* 
       There is we process all stages, which was skipped in fast_sort_,
       because of work_grp > loc_ size
    */

    for (; cur_stage < num_of_pairs; ++cur_stage) 
    {
        for (uint passed_stage = 0; passed_stage < cur_stage + 1; ++passed_stage) 
        {
            try
            {
                //! Setting args for execution simple_sort_
                simple_sort_.setArg(0, buffer);
                simple_sort_.setArg(1, cur_stage);
                simple_sort_.setArg(2, passed_stage);
                simple_sort_.setArg(3, static_cast<unsigned>(dir));
                

                //! Same
                if (!kernel_exec(simple_sort_, glob_size, loc_size, event))
                    throw std::runtime_error{"Execution of simple_sort wasn't sucsessful!\n"};
            }

            catch (cl::Error& err)
            {
                std::cerr << "Error occured in " << err.what() << std::endl;
                std::cerr << err_what(err.err()) << std::endl;
            }
        }
        event.wait();
    }
    //Getting sorted buf with help mapping cl::Buffer

    cl::copy(queue_, buffer, vec.begin(), vec.end());

#if TIME
    std::cout << "bsort gpu_time: "<< timer_gpu.elapsed() << " microseconds\n";
#endif

    vec.resize(old_vec_size);
} /* End of 'sort_extended' function */

/**
 * @brief Hepler function for 'sort extended'
 *        This function extend our vector to near number, which are power of a two
 *
 * @param vec
 * @param dir
 */
void BSort::Vec_preparing(std::vector<int> &vec, Dir dir)
{
  int num_for_fill = dir == Dir::INCR ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();

  size_t old_vs = vec.size();
  size_t new_vs = std::pow(2, 1 + static_cast<int>(log2(old_vs)));

  vec.resize(new_vs);
  auto vb = vec.begin();
  std::fill(vb + old_vs, vb + new_vs, num_for_fill);

} /* End of 'Vec_preparing' function*/


/**
 * @brief Function for execution kernel
 *
 * @param kernel
 * @param global_size
 * @param local_size
 * @return true
 * @return false
 */
bool BSort::kernel_exec(cl::Kernel kernel, size_t global_size, size_t local_size, cl::Event& event)
{
  int err_num = queue_.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size, nullptr, &event);

  if (err_num != CL_SUCCESS)
    return false;

  return true;
} /* End of 'kernel_exec' function*/

} // namespace BTS
