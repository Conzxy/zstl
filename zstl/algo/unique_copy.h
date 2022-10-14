/** 
 * @brief leave the first element of every consecutive sequence where all elements are same("unique")
 * @param first @param last bounds of a range
 * @param result left bound of other range or output iterator
 * @return 
 * end iterator of the second range( @p result )
 */
template<typename II, typename OI>
OI unique_copy(II first, II last, OI result) {
  auto _result = first;
  ++first;

  for (; first != last; ++first) {
    if (!(*_result == *first) && ++_result != first) {
      *result++ = STL_MOVE(*first);
    }
  }

  return result;
}