template<typename BI, 
  STL_ENABLE_IF((Conjunction<
        is_bidirectional_iterator<BI>,
        Negation<is_forward_iterator<BI>>>::value), char)>
BI rotate(BI first, BI mid, BI last) {
  if (first == mid) {
    return last;
  }
  else if (mid == last) {
    return first;
  }
  
  // To bidirectional iterator, we can do following:
  // 1) reverse(first, mid)
  // 2) reverse(mid, last)
  // 3) reverse(first, last)
  zstl::reverse(first, mid);
  zstl::reverse(mid, last);
  
  // Reverse and find returned iterator 
  // Get a remaining sequence does not reversed

  while (first != mid && mid != last) {
    iter_swap(first++, --last);
  }
  
  if (first == mid) {
    // mid is in the left-half range
    zstl::reverse(first, last);
    return last;
  }
  else if (mid == last) {
    // mid is in the right-half range
    zstl::reverse(first, last);
    return first;
  }

}

template<typename RI, 
  STL_ENABLE_IF((Conjunction<
      Negation<is_bidirectional_iterator<RI>>,
      is_random_access_iterator<RI>>::value), long)>
RI rotate(RI first, RI mid, RI last) {
  // To random access iterator,
  // optimizations have:
  // 1) even element count and mid is the midpoint of the range, we can call swap_ranges() simply
  // 2) make distance(first, mid) always less than half length by reverse handling
  // 3) if distance(first, mid) is just one, we can call move() to handle
  
  if (first == mid) {
    return last;
  }
  else if (mid == last) {
    return first;
  }

  const auto n = last - first;
  auto k = mid - first;

  if (n == 2 * k) {
    swap_ranges(first, mid, last);
    return mid;
  }
  
  // note this is inplace rotate 
  auto ret = first + n - k; 

  using ValueType = Iter_value_type<RI>;
  using Diff = Iter_diff_type<RI>;

  for (;;) {
    if (k < n - k) {
      // mid is in the left-half of the range
      // No need use constexpr if here since 
      // there is no instantiation error
      if (zstl::Is_trivial<ValueType>::value &&
          k == 1) {
        auto tmp = STL_MOVE(*first);
        zstl::move(first+1, last, first);
        *--last = STL_MOVE(*first);
        
        return ret;
      }
      
      for (Diff i = 0; i < n - k; ++i) {
        iter_swap(first++, mid);
      }
      
      
      if (!(n %= k)) {
        return ret;
      }

      swap(n, k);
      k = n - k;
    }
    else {
      // mid is in the right-half of the range
      k = n - k;
      if (zstl::Is_trivial<ValueType>::value && 
          k == 1) {
        auto tmp = STL_MOVE(*(last-1));
        zstl::move(first, last-1, first+1);
        *first = STL_MOVE(tmp);

        return ret;
      }

      for (Diff i = 0; i < n - k; ++i) {
        iter_swap(--mid, --last);
      }
      
      if (!(n %= k)) {
        return ret;
      }

      swap(n, k); 
    }
  } 
}