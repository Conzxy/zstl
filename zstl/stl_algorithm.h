#ifndef ZSTL_STL_ALGORITHM_H
#define ZSTL_STL_ALGORITHM_H

#include "config.h"
#include "stl_move.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_algobase.h"
#include "functional.h"


namespace zstl {

// Non-modifying sequence operations 
#include "algo/find.h"
#include "algo/range_check.h"
#include "algo/count.h"
#include "algo/for_each.h"
#include "algo/mismatch.h"
#include "algo/search.h"
#include "algo/find_end.h"

// Modifying sequence operations
#include "algo/copy_if.h"
#include "algo/transform.h"
#include "algo/generate.h"
#include "algo/remove.h"
#include "algo/remove_copy.h"
#include "algo/unique.h"
#include "algo/unique_copy.h"
#include "algo/replace.h"
#include "algo/replace_copy.h"
#include "algo/swap_ranges.h"
#include "algo/reverse.h"
#include "algo/rotate.h"

// Binary Search operation
// (on sorted sequence)
#include "algo/binary_search.h"



} // namespace zstl

#endif //ZSTL_STL_ALGORITHM_H