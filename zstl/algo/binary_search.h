/**
 * @brief binary search
 * @param first @param last bounds of a range
 * @param val value which want to find
 * @retval 
 * true: find
 * false: otherwise
 * @note O(lgn)
 */
template<typename FI, typename T,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
bool binary_search(FI first, FI last, T const val) {
  using Diff = Iter_diff_type<FI>;

  Diff n = distance(first, last);
  Diff half = n / 2;

  while (n > 0) {
    auto mid = advance_iter(first, half);

    if (*mid < val) {
      first = ++mid;
      n -= half + 1;
    }
    else if (*mid > val) {
      n = half;
    }
    else {
      return true;
    }
  }

  return false;
}

/**
 * @brief find the first iterator whose value >= val
 * @param first @param last bounds of a range
 * @param val value in [ @p first , @p last ) which should >=
 * @return see brief
 */
template<typename FI, typename T,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
FI lower_bound(FI first,FI last,T const& val) {
  using Diff = Iter_diff_type<FI>;

  Diff n = distance(first, last);
  Diff half = n / 2;

  while (n > 0) {
    auto mid = advance_iter(first, half);

    if (*mid < val) {
      first = ++mid;
      n -= half + 1;
    }
    else {
      n = half;
    }
  }

  return first;
}

/**
 * @brief Like binary_search() @see binary_search()
 * @param first @param last bounds of a range
 * @param val value which want to find
 * @return 
 * pair:
 * if the .first of pair is true, indicates @p val found, the .second of pair is the iterator,
 * otherwise, indicates not found, the .second of pair is the insert position that 
 * make sequence retain sorted
 * @note 
 * This is not STL-builtin function.
 */
template<typename FI, typename T,
  STL_ENABLE_IF((is_forward_iterator<FI>::value), int)>
zstl::pair<bool, FI> binary_search_iter(FI first, FI last, T const& val) {
  using RT = zstl::pair<bool, FI>;
  auto result = lower_bound(first, last, val);

  return RT{*result == val, result};
}
