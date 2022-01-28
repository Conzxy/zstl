#include_directories(C://lib//fmt//include)

string(REPLACE ";" " " LIB_SRC_LIST "${LIB_SRC}")

message(STATUS "${LIB_SRC_LIST}")

add_executable(vector_test vector_test.cc ${LIB_SRC})
add_executable(ebco_helper_test ebco_helper_test.cc)

# hash table test
add_executable(hash_aux_test hash_aux_test.cc ${LIB_SRC})
add_executable(hashtable_test hashtable_test.cc ${LIB_SRC})

# rbtree
add_executable(set_test set_test.cc ${LIB_SRC})
add_executable(set_bench set_bench.cc ${LIB_SRC})

target_link_libraries(set_test gtest)
target_link_libraries(set_bench benchmark)

# algorithm
add_executable(stl_algorithm_test stl_algorithm_test.cc)
target_link_libraries(stl_algorithm_test gtest)

# iterator
add_executable(iterator_test iterator_test.cc)
