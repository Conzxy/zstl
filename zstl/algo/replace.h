/**
 * @brief replace @p old_value in a range with @p new_value
 * @param first @param last bounds of a range
 * @param old_value see brief
 * @param new_value see brief
 */
template<typename FI, typename T,
  STL_ENABLE_IF((Conjunction<
        is_forward_iterator<FI>,
        Is_same<Iter_value_type<FI>, T>>::value), int)>
void replace(FI first, FI last, T const& old_value, T const& new_value) {
  for (; first != last; ++first) {
    if (*first == old_value) {
      *first = new_value;
    }
  }
}

/**
 * @brief replace values which satisfy @p pred with @p new_value
 * @param first @param last bounds of a range
 * @param pred unary predicate that old value should satisfy
 * @param new_value see brief
 */
template<typename FI, typename UnaryPred, typename T,
  STL_ENABLE_IF((Conjunction<
        is_forward_iterator<FI>,
        Is_same<Iter_value_type<FI>, T>>::value), int)>
void replace_if(FI first, FI last, UnaryPred pred, T const& new_value) {
  for (; first != last; ++first) {
    if (pred(*first)) {
      *first = new_value;
    }
  }
}
