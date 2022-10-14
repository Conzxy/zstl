
/**
 * @brief Apply the given function to a range and store the result
 * in another range.
 * @param first:  left bound of transformd range
 * @param last:   right bound of transformed range
 * @param result: another range which stores the result
 * @param func:   transform operation which must be unary callable
 * @return 
 * the iterator which past the last transformed element
 * (Since result range maybe over given range)
 */
template<typename II, typename OI, typename UnaryFunc>
ZSTL_CONSTEXPR OI 
transform(
    II first, II last, 
    OI result,
    UnaryFunc func) {
  for (; first != last; ++first) {
    *result++ = func(*first);
  }

  return result;
}

/**
 * @brief 
 * Like transform above, but operation take two arguments 
 * to transform and store result in anoter range.
 * @param first:  left bound of first range
 * @param last:   right bound of first range
 * @param first2: first bound of the second range
 * @param result: another range which stores the result
 * @param func:   transform operation which must be binary callable
 * @return 
 * the iterator which past the last transformed element
 * (Since result range maybe over given range)
 *
 */
template<typename II, typename OI, typename II2, typename BinFunc>
ZSTL_CONSTEXPR OI transform(
    II first, II last,
    II2 first2,
    OI result,
    BinFunc func) {
  for (; first != last; ++first, ++first2) {
    *result++ = func(*first, *first2);
  }

  return result;
}