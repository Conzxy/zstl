// mismatch
/**
 * @brief find the first mismatch pair exists between two ranges(values match)
 * @param first1 left bound of range
 * @param last1  right bound of range
 * @param first2 left bound of other range
 * @warning
 * If the second range is shorter than the first, it maybe occur UB.
 * @return
 * the first mismatch pair, last if there is no mismatch pair.
 */
template<typename II1, typename II2>
ZSTL_CONSTEXPR zstl::pair<II1, II2>
mismatch(II1 first1, II1 last1, 
         II2 first2) {
  for (; first1 != last1 && *first1 == *first2;
       ++first1, ++first2);

  return zstl::make_pair(first1, first2);
}

/**
 * @brief find the first mismatch pair exists between two ranges(use predicate to match)
 * @param first1 left bound of range
 * @param last1  right bound of range
 * @param first2 left bound of other range
 * @param pred   binary predicate which test whether is match
 * @warning
 * If the second range is shorter than the first, it maybe occur UB.
 * @return
 * the first mismatch pair, last if there is no mismatch pair.
 */
template<typename II1, typename II2, typename BinPred>
ZSTL_CONSTEXPR zstl::pair<II1, II2>
mismatch(II1 first1, II1 last1,
         II2 first2, BinPred pred) {
  for (; first1 != last1 && pred(*first1, *first2);
       ++first1, ++first2);

  return zstl::make_pair(first1, first2);
}