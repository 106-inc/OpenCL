/**
 * simple_sort - its kernel - simple realisation bitonic sort without local memory
 */
__kernel void simple_sort(__global int * vec, uint cur_pair, uint passed_pair, uint dir)
{
    uint id = get_global_id(0);
    
    uint pair_distance = 1 << (cur_pair - passed_pair);

    uint left_id = (id % pair_distance) + (id / pair_distance) * 2 * pair_distance;
    uint right_id = left_id + pair_distance;
    
    int left_elem = vec[left_id];
    int right_elem = vec[right_id];
    
    if (( id / (1 << cur_pair)) % 2 == 1 )
        dir = 1 - dir;

    int greater = (left_elem > right_elem) ? left_elem : right_elem;
    int lesser = (left_elem > right_elem) ? right_elem : left_elem;

    vec[left_id] = dir ? lesser : greater;
    vec[right_id] = dir ? greater : lesser;
}



/**
 * fast_sort - it's kernel - simple realisation bitonic sort with local memory
 */
__kernel void fast_sort(__global int* vec, uint cur_pair, __local int* local_data, uint direction) 
{
    
    //! Initializing data for allocation memory
    uint local_id  = get_local_id(0);
    uint dir = direction;
    uint group_size = get_local_size(0);
    uint offset  = get_group_id(0) * group_size;

    //! Here we just copy our vec to the work grp in pairs from global memory
    local_data[local_id] = vec[offset * 2 + local_id];
    local_data[local_id + group_size] = vec[offset * 2 + local_id + group_size];
    
    //! Here we just waiting for all threads
    //! Then coppy full vec
    //waiting for all threads -> copyied full array
    barrier(CLK_LOCAL_MEM_FENCE);

    //! There is we use simple sort from top "biton.cl"
    //! Compare two elems and swap it or skip
    for(uint pair = 0; pair < cur_pair; ++pair) 
    {
        for(uint pair_passed = 0; pair_passed < pair + 1; ++pair_passed) 
        {
            uint compare_distance = 1 << (pair - pair_passed);

            uint left_id = (local_id % compare_distance) + (local_id / compare_distance) * 2 * compare_distance;
            uint right_id = left_id + compare_distance;
    
            int left_elem = local_data[left_id];
            int right_elem = local_data[right_id];
    
            if((local_id / (1 << pair)) % 2 == 1)
                dir = 1 - dir;

            int greater = right_elem;
            int lesser = left_elem;

            greater = (left_elem > right_elem) ? left_elem : right_elem;
            lesser = (left_elem > right_elem) ? right_elem : left_elem;
    
            
            //! There is we should check direction our sortion
            local_data[left_id] = dir ? lesser : greater;
            local_data[right_id] = dir ? greater : lesser;

            dir = direction;

            //! Here we waiting for all pairs of comparision
            barrier(CLK_LOCAL_MEM_FENCE);
        }
    }

    //! FINALLY we copy memory back
    //! Copy from local memory to glb memory
    vec[offset * 2 + local_id] = local_data[local_id];
    vec[offset * 2 + local_id + group_size] = local_data[local_id + group_size];
}