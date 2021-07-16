#include_directories(C://lib//fmt//include)

message(${LIB_SRC})
#add_executable(test_queue test_queue.cpp ${LIB_SRC})
add_executable(test_vector test_vector.cpp ${LIB_SRC})
#add_executable(test_algo test_algo.cpp ${LIB_SRC})
#add_executable(test_hashtable test_hashtable.cpp ${LIB_SRC})
#add_executable(test_other test_other.cpp ${LIB_SRC})
add_executable(rbtree rbtree.cc ${LIB_SRC})
