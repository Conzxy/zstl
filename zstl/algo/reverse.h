/**
 * @brief reverse the elements in a range
 * @param first @param last bounds of a range
 */
template<typename BI, 
  STL_ENABLE_IF((
    is_bidirectional_iterator<BI>::value), int)>
void reverse(BI first, BI last){
  if(first == last || first == --last)
    return ;
  else {
    // first != last used for element count is even number
    // first != --last used for element count is odd number
    while (first != last && first != --last)
      iter_swap(first++,last);
  }
}

template<typename RI,
  STL_ENABLE_IF((Conjunction<
    Negation<is_bidirectional_iterator<RI>>,
    is_random_access_iterator<RI>>::value), char)>
void reverse(RI first, RI last){
  // No need to distinguish element count whther is even number or odd
  while(first < last)
    iter_swap(first++,--last);
}

/**
 * @brief rotate [ @p first, @p last ) ==> [mid, last) before [first, mid)
 * @param first @param last bounds of a range
 * @param mid ratate point, it will be new first
 * @return 
 * @p first position, i.e. first + distance( @p mid , @p last )
 */
template<typename FI,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
FI rotate(FI first, FI mid, FI last) {
  if (first == mid) {
    // last might be not valid, if sequence is circular, *last == *first
    return last;
  } 
  else if (mid == last) {
    return first;
  }

  auto first2 = mid; 
  // Let k = distance(first, mid).
  // Find the first range whose length is less than k
  for (; first2 != last; ++first2) {
    iter_swap(first++, first2);
    if (first == mid) {
      mid = first2;
    }
  }
  
  // first2 advances distance(mid, last), 
  // so does first.
  auto ret = first;

  // rotate remaining sequence
  // 1) recurrsion method
  // rotate(first, mid, last);


  // 2) loop
  first2 = mid;
  for (; first2 != last; ) {
    iter_swap(first++, first2++);
    if (first == mid) {
      mid = first2;
    }
    else if (first2 == last) 
    {
      first2 = mid;
    }
  }

  return ret;
}