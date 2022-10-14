/**
 * @brief swap element in two range
 * @param first @param last bounds of the first range
 * @param result left bounds of other range
 * @warning 
 * length of the second range should above or equal the first range,
 * UB otherwise.
 * @return 
 * end iterator of the second range, 
 * it should be result + distance( @p first , @p last )
 */
template<typename FI1, typename FI2,
  STL_ENABLE_IF((Conjunction<
        is_forward_iterator<FI1>,
        is_forward_iterator<FI2>>::value), int)>
FI2 swap_ranges(FI1 first, FI1 last,
                FI2 result) {
  for (; first != last; ++first) {
    iter_swap(first, result++);
  } 

  return result;
}