/**
 * @brief 
 * Assign element in given range a value which
 * generated by given no arguments callable
 * @param first left bound of range
 * @param last  right bound of range
 * @param gen   generator which must be no argument callable
 * @note 
 * This is suitable for cases where the bounds of a range are known
 * (e.g. defined container variable)
 */
template<typename FI, typename Gen>
ZSTL_CONSTEXPR void 
generate(FI first, FI last, Gen gen) {
  for (; first != last; ++first) {
    *first = gen();
  }
}

/**
 * @brief 
 * Assign element to begin of a range in given count
 * @param first first element in a range
 * @param sz    count of generated values
 * @param gen   generator which must be no argument callable
 * @note 
 * This is suitable for cases where element count is known
 * (e.g. insert_iterator(inserter) or stream_iterator)
 */
template<typename FI, typename SZ, typename Gen>
ZSTL_CONSTEXPR void
generate_n(FI first, SZ sz, Gen gen) {
  for (SZ i = 0; i < sz; ++i) {
    *first++ = gen();
  }
}
