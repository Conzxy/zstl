namespace detail {


// The inverse version of search
template<typename FI1, typename FI2>
FI1 find_end_aux(FI1 first1, FI1 last1,
                 FI2 first2, FI2 last2,
                 std::forward_iterator_tag,
                 std::forward_iterator_tag) {
  auto result = last1;
  for (; first1 != last1; ++first1) {
    auto search_result = search(first1, last1, first2, last2);

    if (search_result != last1) {
      // Update result until it is fixed
      result = search_result; 
      first1 = search_result;
    } 
    else {
      break;
    }

  }

  return result;
}

// Use reverse iterator to find first occurrence
template<typename BI1, typename BI2>
BI1 find_end(BI1 first1, BI1 last1,
             BI2 first2, BI2 last2,
             std::bidirectional_iterator_tag,
             std::bidirectional_iterator_tag) {

  // Use reverse_iterator to implement reverse search
  using RevIter1 = zstl::reverse_iterator<BI1>;
  using RevIter2 = zstl::reverse_iterator<BI2>;

  auto result = zstl::search(RevIter1(last1), RevIter1(first1),
                       RevIter2(last2), RevIter2(first2));
  
  if (result == RevIter1(first1)) {
    return last1; 
  } 
  else {
    return advance_iter(result.base(), -zstl::distance(first2, last2));
  }
}

} // namespace detail

/**
 * @brief find the last occurrence of the second range in the first range
 * @param first1 @param last1 the first range, these must be forward or bidirection iterator
 * @param first2 @param last2 the second range like @p first1 and @p last1
 * @return 
 * return last occurrence iterator if find, last otherwise
 */
template<typename Iter1, typename Iter2>
ZSTL_CONSTEXPR Iter1 
find_end(Iter1 first1, Iter1 last1,
         Iter2 first2, Iter2 last2)
{
  return detail::find_end(
    STL_MOVE(first1), STL_MOVE(last1),
    STL_MOVE(first2), STL_MOVE(last2),
    zstl::Iter_category<Iter1>(),
    zstl::Iter_category<Iter2>());
}
