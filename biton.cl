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
