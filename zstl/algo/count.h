
// count, count_if
/**
 * @brief count the number of elements in the given range whose value equal to 
 * the given value
 * which equal to the given value
 * @param first left bound of range
 * @param last  right bound of range
 * @param val   value that want to count
 * @retval 
 * see brief
 */
template<typename II, typename T>
ZSTL_CONSTEXPR zstl::Iter_diff_type<II>
count(II first, II last, T const& val) {
  using Diff = zstl::Iter_diff_type<II>;
  Diff cnt = 0;

  for (; first != last; ++first) {
    cnt += static_cast<Diff>(*first == val);
  }

  return cnt;
}

/**
 * @brief Like count(), count the number of elements in the given range 
 * that satisfy the given criterion
 * @param first left bound of range
 * @param last  right bound of range
 * @param pred  unary predicate
 * @retval
 * see brief
 */
template<typename II, typename UnaryPred>
ZSTL_CONSTEXPR Iter_diff_type<II>
count_if(II first, II last, UnaryPred pred) {
  using Diff = zstl::Iter_diff_type<II>;
  Diff cnt = 0;

  for (; first != last; ++first) {
    cnt += static_cast<Diff>(pred(*first));
  }

  return cnt;
}