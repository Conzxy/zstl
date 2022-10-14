
/**
 * @brief Like remove() but not inplace modify, instead of storing result in another sequence
 * @param first: left bound of a sequence
 * @param last:  right bound of a sequence
 * @param result:left bound of another sequence or output iterator
 * @param val:   value in sequence that should removed
 * @return 
 * End iterator of another sequence which is same with old sequence except values which equal to given value all have removed.
 */
template<typename II, typename OI, typename T>
OI remove_copy(II first, II last, OI result, T const& val) {
  for (; first != last; ++first) {
    if (!(*first == val)) {
      *result++ = STL_MOVE(*first);
    }
  }

  return result; 
}

/**
 * @brief Like remove_if(), but not inplace modify, instead of storing result in another sequence
 * @param first: left bound of a sequence
 * @param last:  right bound of a sequence
 * @param result:left bound of a sequence or output iterator
 * @param pred:  unary predicate which removed values satisfy
 * @return 
 * End iterator of another sequence which is same with old sequence except 
 * values which satisfy the given predicate all have removed.
 */
template<typename II, typename OI, typename UnaryPred>
OI remove_copy_if(II first, II last, OI result, UnaryPred pred) {
  for (; first != last; ++first) {
    if (!pred(*first)) {
      *result++ = STL_MOVE(*first);
    }
  }

  return result;
}