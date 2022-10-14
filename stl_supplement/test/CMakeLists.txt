# forward_list test
add_executable(forward_list_test forward_list_test.cc ${FORWARD_LIST_SRC})
target_link_libraries(forward_list_test gtest)

add_executable(forward_list_bench forward_list_bench.cc ${FORWARD_LIST_SRC})
target_link_libraries(forward_list_bench gtest benchmark)

# output_buffer_test
add_executable(output_buffer_test output_buffer_test.cc ${SRC_PATH}/output_buffer.cc ${FORWARD_LIST_SRC})

# string_view_test
add_executable(string_view_test string_view_test.cc ${SRC_PATH}/string_view.cc)
target_link_libraries(string_view_test gtest)
