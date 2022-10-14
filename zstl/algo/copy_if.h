/**
 * @brief Like copy() but use predicate to copy element
 * @param first @param last bounds of range
 * @param result left bound of other range or output iterator(e.g. inserter or stream_iterator)
 * @param pred   unary predicate which copyed element should satisfy
 * @return 
 * end iterator of the second range
 * @note 
 * since every element should be test, so it's impossible to specialize copy_if() like copy()
 */
template<typename II, typename OI, typename UnaryPred>
OI copy_if(II first, II last, OI result, UnaryPred pred) {
  for (; first != last; ++first) {
    if (pred(*first)) {
      result++ = *first;
    }
  } 

  return result;
}