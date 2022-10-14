/**
 * @brief remove all values which equal to given value by shifting to tail of sequence
 * @param first: left bound of a sequence
 * @param last:  right bound of a sequence
 * @param val:   value in sequence that should removed
 * @return 
 * logic end of new sequence where no values equal to given value
 * (Normally, You can call corresponding erase() method of container such that remove these values actually)
 */
template<typename FI, typename T, 
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
FI remove(FI first, FI last, T const& val) {
  // The implementation IDEA: double pointer
  // Maintains a loop invariant: The elements in [l, r) that contains the val only
  // when loop is terminated, the old sequence becomes:
  // +----------------+----------------+
  // | other elements | elements = val |
  // +----------------+----------------+
  //
  // If A[r] doesn't equal val, swap A[l] and A[r], then in the next loop,
  // the [l, r) still contains val only
  first = find(first, last, val);
  auto result = first;
  for (; first != last; ++first) {
    if (!(*first == val)) {
      // *result++ = STL_MOVE(*first);
      zstl::iter_swap(first, result);
    }
  }

  return result; 
}

/**
 * @brief remove all values which satisfy the given predicate by shifting to tail of sequence
 * @param first: left bound of a sequence
 * @param last:  right bound of a sequence
 * @param pred:  unary predicate which removed values satisfy
 * @return 
 * logic end of new sequence where no values equal to given value
 * (Normally, You can call corresponding erase() method of container such that remove these values actually)
 */
template<typename FI, typename UnaryPred,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
FI remove_if(FI first, FI last, UnaryPred pred) {
  first = find_if(first, last, pred);
  auto result = first;

  for (; first != last; ++first) {
    if (!pred(*first)) {
      *result++ = STL_MOVE(*first);
    }
  }

  return result;
}
