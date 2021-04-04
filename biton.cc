#include "biton.hh"

namespace BTS 
{

/**
 * @brief 
 * 
 * @param vec 
 * @param dir 
 */
void bsort(std::vector<int>& vec, Dir dir)
{
    BSort ctor{};
    ctor.sort_extended(vec, dir);
} /* End of 'bsort' function */


/**
 * @brief Construct a new BSort::BSort object function
 *
 */
BSort::BSort()
{
    Device_selection();

    work_group_size = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

    context_ = cl::Context({device_});
    queue_ = cl::CommandQueue(context_, device_);

    build();

    return;
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

    throw std::invalid_argument("No devices found");
}  /* End of 'Device_selection' function */


/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool BSort::build()
{   
    std::ifstream src(kernel_file_);

    if (!src.is_open())
    return false;

    src_code_ = {std::istreambuf_iterator<char>(src), std::istreambuf_iterator<char>()};

    sources_ = cl::Program::Sources(1, std::make_pair(src_code_.c_str(), src_code_.length() + 1));

    prog_ = cl::Program(context_, sources_);
    prog_.build();

    simple_sort_ = cl::Kernel(prog_, "simple_sort");

    return true;
} /* End of 'build' function */


/**
 * @brief 
 * 
 * @param vec 
 * @param dir 
 */
void BSort::sort_extended(std::vector<int> &vec, Dir dir) 
{
    size_t old_vec_size = vec.size();
    Vec_preparing(vec, dir);
    size_t new_vec_size = vec.size();

    size_t work_grp_sze = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    size_t glob_size = vec.size() / 2;
    size_t loc_size = std::min(glob_size, work_grp_sze);

    cl::Buffer buffer(context_, CL_MEM_READ_WRITE, sizeof(int) * vec.size());
    queue_.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(int) * vec.size(), vec.data());

    int num_of_pairs = std::ceil(std::log2(new_vec_size));
    int cur_pair = std::log2(loc_size);

    /*cl::LocalSpaceArg local = cl::Local(2 * loc_size * sizeof(int));*/

    simple_sort_.setArg(0, buffer);
    simple_sort_.setArg(3, dir);

    for (; cur_pair < num_of_pairs; ++cur_pair) 
    {
        simple_sort_.setArg(1, cur_pair);

        for (int passed_pair = 0; passed_pair < cur_pair + 1; ++passed_pair) 
        {
            simple_sort_.setArg(2, passed_pair);
            kernel_exec(simple_sort_, glob_size, loc_size);
        }
    }

    auto mapped_vec = (int *)queue_.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_READ, 0, new_vec_size * sizeof(int));

        for (size_t i = 0; i < new_vec_size; i++)
            vec[i] = mapped_vec[i];
        
    queue_.enqueueUnmapMemObject(buffer, mapped_vec);

    vec.resize(old_vec_size);
} /* End of 'sort_extended' function */

/**
 * @brief 
 * 
 * @param vec 
 * @param dir 
 */
void BSort::Vec_preparing(std::vector<int>& vec, Dir dir)
{
    size_t old_vec_size = vec.size();
    size_t new_vec_size = std::pow(2,1 + static_cast<int>(log2(old_vec_size)));

    int pushing_num = 0;
    
    if (dir == Dir::INCR)
        pushing_num = std::numeric_limits<int>::max();

    else
        pushing_num = std::numeric_limits<int>::min();

    vec.reserve(new_vec_size);
    for (size_t i = old_vec_size; i < new_vec_size; ++i)
        vec.push_back(pushing_num);
} /* End of 'Vec_preparing' function*/


/**
 * @brief 
 * 
 * @param kernel 
 * @param global_size 
 * @param local_size 
 * @return true 
 * @return false 
 */
bool BSort::kernel_exec(cl::Kernel kernel, size_t global_size, size_t local_size)
{
    cl::Event event;
    int err_num = queue_.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size, nullptr, &event);

    if (err_num != CL_SUCCESS)
        return false;

    event.wait();
    return true;
}

} /* End of 'kernel_exec' function*/

