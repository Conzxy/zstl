
// C++11:
// all_of, any_of, none_of
/**
 * @brief test whether all elements in the given range satisfy the given criterion
 * @param first left bound of range
 * @param last  right bound of range
 * @param p     unary predicate whose argument type should be same with element of the range
 * @retval
 * true:  if all elements in the [ @p first , @p last ) range satisfy @p p
 * false: otherwise
 */
template<typename II, typename UnaryPred>
ZSTL_CONSTEXPR bool 
all_of(II first, II last, UnaryPred p)  {
  // All satisfy predicate is equivalent to none does not satisfy predicate
  return find_if_not(first, last, p) == last;
}  

/**
 * @brief test whether has at least one element in the given range satisfy the given criterion
 * @param first left bound of range
 * @param last  right bound of range
 * @param p     unary predicate whose argument type should be same with element of the range
 * @retval
 * true:  if at least one element in the [ @p first , @p last ) range satify @p p
 * false: otherwise
 */
template<typename II, typename UnaryPred>
ZSTL_CONSTEXPR bool 
any_of(II first, II last, UnaryPred p)  {
  return find_if(first, last, p) != last;
}

/**
 * @brief test whether none in the given range satisfy the given criterion
 * @param first left bound of range
 * @param last  right bound of range
 * @param p     unary predicate whose argument type should be same with element of the range
 * @retval 
 * true:  if all elements in the [ @p first , @p last ) range satisfy @p p
 * flase: otherwise
 */
template<typename II, typename UnaryPred>
ZSTL_CONSTEXPR bool 
none_of(II first, II last, UnaryPred p)  {
  return find_if(first, last, p) == last;
  // i.e. 
  // return !any_of(first, last, p);
}
