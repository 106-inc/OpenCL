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
    BSort::driver().sort_extended(vec, dir);
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
}  /* End of 'Device_selection' function */


/**
 * @brief build - helper function for constructor: creating any members of class
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
    fast_sort_ = cl::Kernel(prog_, "fast_sort");

    src.close();

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

    //! Getting the size of the ND range space that can be handled by a single invocation of a kernel compute unit. 
    size_t work_grp_sze = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

    //! global_size <=> number of work-items that I wish to execute
    size_t glob_size = vec.size() / 2;

    //! local_size <=> number of work-items that I wish to group into a work-group
    //! size of loc_size should be less or equal work_group
    //! This is the reason of comparing elems on this distance
    size_t loc_size = std::min(glob_size, work_grp_sze);

    //! Creating special buffer for working with glod memory in kernel
    cl::Buffer buffer(context_, CL_MEM_READ_WRITE, sizeof(int) * vec.size());
    queue_.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(int) * vec.size(), vec.data());

    //! Getting number of pairs of our resized vector
    int num_of_pairs = std::ceil(std::log2(new_vec_size));

    int cur_pair = std::log2(loc_size);

    //! Allocation local memory for working in fast_sort_
    cl::LocalSpaceArg local = cl::Local(2 * loc_size * sizeof(int));

    Time::Timer timer;

    //! Setting args for execution fast_sort_
    try
    {
        fast_sort_.setArg(0, buffer);
        fast_sort_.setArg(1, cur_pair);
        fast_sort_.setArg(2, local);
        fast_sort_.setArg(3, static_cast<unsigned>(dir));

        //! fast_sort_ execution
        if (!kernel_exec(fast_sort_, glob_size, loc_size))
            throw std::runtime_error{"Execution of simple_sort wasn't sucsessful!\n"};
    }
    catch (cl::Error& err)
    {
        std::cerr << "Error occured in " << err.what() << std::endl;
        std::cerr << err_what(err.err()) << std::endl;
    }


    /* 
       There is we sort pairs, which was skipped in fast_sort_,
       because of work_grp > loc_ size
    */
    for (; cur_pair < num_of_pairs; ++cur_pair) 
    {
        for (int passed_pair = 0; passed_pair < cur_pair + 1; ++passed_pair) 
        {
            try
            {
                //! Setting args for execution simple_sort_
                simple_sort_.setArg(0, buffer);
                simple_sort_.setArg(1, static_cast<unsigned>(cur_pair));
                simple_sort_.setArg(2, static_cast<unsigned>(passed_pair));
                simple_sort_.setArg(3, static_cast<unsigned>(dir));
                

                //! Same
                if (!kernel_exec(simple_sort_, glob_size, loc_size))
                    throw std::runtime_error{"Execution of simple_sort wasn't sucsessful!\n"};
            }

            catch (cl::Error& err)
            {
                std::cerr << "Error occured in " << err.what() << std::endl;
                std::cerr << err_what(err.err()) << std::endl;
            }
        }
    }

    //Getting sorted buf with help mapping cl::Buffer
    auto mapped_vec = static_cast<int*>(queue_.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_READ, 0, new_vec_size * sizeof(int)));

    for (size_t i = 0; i < new_vec_size; ++i)
        vec[i] = mapped_vec[i];
    
    queue_.enqueueUnmapMemObject(buffer, mapped_vec);

    std::cout << "bsort time: "<< timer.elapsed() << " microseconds\n";
    vec.resize(old_vec_size);
} /* End of 'sort_extended' function */



/**
 * @brief Hepler function for 'sort extended'
 *        This function extend our vector to near number, which are power of a two
 * 
 * @param vec 
 * @param dir 
 */
void BSort::Vec_preparing(std::vector<int>& vec, Dir dir)
{
    size_t old_vec_size = vec.size();
    size_t new_vec_size = std::pow(2,1 + static_cast<int>(log2(old_vec_size)));

    int num_for_fill = 0;
    
    if (dir == Dir::INCR)
        num_for_fill = std::numeric_limits<int>::max();

    else
        num_for_fill = std::numeric_limits<int>::min();

    vec.reserve(new_vec_size);

    for (size_t i = old_vec_size; i < new_vec_size; ++i)
        vec.push_back(num_for_fill);
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
bool BSort::kernel_exec(cl::Kernel kernel, size_t global_size, size_t local_size)
{
    cl::Event event;
    int err_num = queue_.enqueueNDRangeKernel(kernel, cl::NullRange, global_size, local_size, nullptr, &event);

    if (err_num != CL_SUCCESS)
        return false;

    event.wait();
    return true;
} /* End of 'kernel_exec' function*/


} 

