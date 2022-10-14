/**
 * @brief Like replace(), but store the result in other range @see replace()
 * @param first @param last bounds of a range
 * @param result left bound of other range, or output iterator
 * @param old_value values in [ @p first , @p last ) which want to replace
 * @param new_value new value replace @p old_value
 * @retval 
 * end iterator of @p result range
 */
template<typename II, typename OI, typename T,
  STL_ENABLE_IF((Conjunction<
      Is_convertible<T, Iter_value_type<II>>,
      Is_convertible<T, Iter_value_type<OI>>>::value), int)>
OI replace_copy(II first, II last, OI result, 
                T const& old_value, T const& new_value) {
  using ValueType2 = Iter_value_type<OI>;
  for (; first != last; ++first) {
    
    *result++ = (*first == old_value) ?
      static_cast<ValueType2>(new_value) : 
      static_cast<ValueType2>(*first);
  }

  return result;
}

/**
 * @brief Like replace_if(), but store the result in other range @see replace_if()
 * @param first @param last bounds of a range
 * @param result left bound of other range, or output iterator
 * @param pred      unary predicate that old value satisfy
 * @param new_value new value replace @p old_value
 * @retval 
 * end iterator of @p result range
 */
template<typename II, typename OI, typename T, typename UnaryPred,
  STL_ENABLE_IF((Conjunction<
      Is_convertible<T, Iter_value_type<II>>,
      Is_convertible<T, Iter_value_type<OI>>>::value), int)>
OI replace_copy_if(II first, II last, OI result,
                   UnaryPred pred, T const& new_value) {
  using ValueType2 = Iter_value_type<OI>;
  for (; first != last; ++first) {
    
    *result++ = pred(*first) ?
      static_cast<ValueType2>(new_value) : 
      static_cast<ValueType2>(*first);
  }

  return result;
}