/**
 * @brief find the first occurrence of the second range in the first range
 * @tparam FI1 the forward iterator of the first range
 * @tparam FI2 the forward iterator of the second range
 * @param first1 @param last1 the first range
 * @param first2 @param last2 the second range
 * @return 
 * the first occurrence iterator if find, @p last1 otherwise
 */
template<typename FI1, typename FI2,
  STL_ENABLE_IF((zstl::Conjunction<
      is_forward_iterator<FI1>,
      is_forward_iterator<FI2>>::value), int)>
FI1 search(FI1 first1, FI1 last1,
           FI2 first2, FI2 last2) {
  for (; first1 != last1; ++first1) {
    if (*first1 == *first2) {
      auto _first1 = first1;
      auto _first2 = first2;
      for (++_first1, ++_first2; 
           _first1 != last1 && _first2 != last2 && 
           *(_first1) == *(_first2);
           ++_first1, ++_first2);
      
      // Match range
      if (_first2 == last2) return first1;
      // The first range is shorter than the second
      // No need
      // if (_first1 == last1) return last1;
      
      first1 = _first1;
    }
  }

  return last1;
}

/**
 * @brief find the first occurrence of the second range in the first range(use predicate to find)
 * @tparam FI1 the forward iterator of the first range
 * @tparam FI2 the forward iterator of the second range
 * @param first1 @param last1 the first range
 * @param first2 @param last2 the second range
 * @param pred binary predicate which element of two ranges should satisfy
 * @return 
 * the first occurrence iterator if find, @p last1 otherwise
 */
template<typename FI1, typename FI2, typename BinPred,
  STL_ENABLE_IF((zstl::Conjunction<
      is_forward_iterator<FI1>,
      is_forward_iterator<FI2>>::value), int)>
FI1 search(FI1 first1, FI1 last1,
           FI2 first2, FI2 last2,
           BinPred pred) {
  for (; first1 != last1; ++first1) {
    if (*first1 == *first2) {
      auto _first1 = first1;
      auto _first2 = first2;
      for (++_first1, ++_first2; 
           _first1 != last1 && _first2 != last2 && 
           pred(*(_first1), *(_first2));
           ++_first1, ++_first2);
      
      // Match range
      if (_first2 == last2) return first1;
      // The first range is shorter than the second
      // No need
      // if (_first1 == last1) return last1;
      
      first1 = _first1;
    }
  }

  return last1;
}