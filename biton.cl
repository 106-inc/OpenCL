__kernel void bitonic_sort(__global int* data, uint cur_pair, uint dist_pair, uint dir)
{
  uint thr_num = get_global_id(0);         /* thread index   */
  uint low = thr_num & (dist_pair - 1);    /* low order bits (below dist_pair) */
  uint i = (thr_num << 1) - low;           /* insert 0 at position dist_pair  */
  bool reverse = ((dir & i) == 0);         /* asc/desc order                  */
  data += i; // translate to first value

  /* Load */
  int x0 = data[0];
  int x1 = data[dist_pair];

  /* Sortion */
  bool swap = reverse ^ (x0 < x1);
  int tmp_x0 = x0;
  int tmp_x1 = x1;
  x0 = (swap) ? tmp_x1 : tmp_x0;
  x1 = (swap) ? tmp_x0 : tmp_x1;

  /* Store  */
  data[0] = x0;
  data[dist_pair] = x1;
}
