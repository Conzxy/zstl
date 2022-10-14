

 // find, find_if, find_if_not(c++11)
/**
 * @brief find the iterator whose value equal to given value
 * @param first left bound of range
 * @param right right bound of range
 * @param val   value that want to find
 * @return 
 * the iterator whose value equal to @p val if find,
 * @p last otherwise
 * @note 
 * @p first and @p last can be input iterator:
 * input iterator has only single pass, but does not indicates that target(retval) is invalid
 * after find, also can be dereference.
 */
template<typename II,typename T>
ZSTL_CONSTEXPR II
find(II first, II last, T const& val) {
  while (first != last && *first != val) ++first;
  return first;
}

/**
 * @brief Like find(), but satisfy given predicate
 * @param first left bound of range
 * @param right right bound of range
 * @param pred  unary predicate which target satisfy
 * @return 
 * the iterator whose value satisfy the @p pred
 * @p last otherwise
 */
template<typename II,typename UnaryPred>
ZSTL_CONSTEXPR II
find_if(II first, II last, UnaryPred pred) {
  while (first != last && !pred(*first)) ++first;
  return first;
}

/**
 * @brief inverse version of find_if() @see find_if()
 * @param first left bound of range
 * @param right right bound of range
 * @param pred  unary predicate which target does not satisfy
 * @return 
 * the iterator whose value does not satisfy the @p pred
 * @p last otherwise
 */
template<typename II, typename Pred>
ZSTL_CONSTEXPR II
find_if_not(II first, II last, Pred pred) {
  while (first != last && pred(*first)) ++first;
  return first;
}
