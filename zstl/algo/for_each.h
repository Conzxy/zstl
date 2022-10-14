
// for_each
// !Check Lambda can return
/**
 * @brief appies given callback to all element in the given range
 * @param first left bound of a range
 * @param last  right bound of a range
 * @param func  callback function whose argument type should can be converted
 * to element type in the range
 * @retval
 * callback argument @p func
 */
template<typename II, typename UnaryFunc>
ZSTL_CONSTEXPR UnaryFunc 
for_each(II first, II last, UnaryFunc func) {
  for (; first != last; ++first) {
    func(*first); 
  }

  return func;
}