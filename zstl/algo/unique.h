/** 
 * @brief leave the first element of every consecutive sequence where all elements are same("unique")
 * @param first @param last bounds of a range
 * @return 
 * logical end iterator, after end(include end) are garbage which should be erased.
 */
template<typename FI,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
FI unique(FI first, FI last) {
  auto result = first;
  ++first;
    
  // The implemetation IDEA: Double pointer
  // Detail: Maintain a window [l+1, r) which contains duplicate elements only(loop invariant)
  // In the last, r becomes last, then the old sequence becomes:
  // | unique elements | duplicate elements |
  // the l+1 is the position of the first duplicate element 
  // In the loop, the l point to the unique element of the the current processing sequence
  // To filter all the duplicate elements, the window starts with l+1 instead of l
  //
  // e.g.  
  // 0 0 0 1 1 2 2 3 4
  // ^ ^
  // init: [l+1, r) is empty
  // skip duplicate elements
  // 
  // 0 0 0 1 1 2 2 3 4 ==> 0 1 0 0 1 2 2 3 4
  // ^     ^                 ^     ^
  // [l+1, r) = { 0, 0 }
  //
  // 0 1 0 0 1 2 2 3 4 ==> 0 1 2 0 1 0 2 3 4
  //   ^       ^               ^       ^
  // [l+1, r) = { 0, 1, 0 }
  // 0 1 2 0 1 0 2 3 4 ==> 0 1 2 3 1 0 2 0 4 ==> 0 1 2 3 4 0 2 0 1
  //     ^         ^             ^         ^             ^        ^
  // termination: [l+1, r) = { 0, 2, 0, 1 }
  for (; first != last; ++first) {
    // // Indicates find a new sequence and the first is the unique element
    // // To maintain the loop variant, we must swap A[l+1] and A[r] if l+1 != r
    // if (!(*result == *first)) {
    //   // Get the first duplicate element
    //   ++result;
    //   // The reason use if is that we cannot assume the ocst of move operation is cheap
    //   if (result != first) {
    //     zstl::iter_swap(result, first);
    //   }
    // }
    // // *result == *first, indicates *first is a duplicate element of *result, don't care
    
    if (!(*result == *first) && ++result != first) {
      zstl::iter_swap(result, first);
      // *result = STL_MOVE(*first);
    }
  }
  
  return ++result;
}
