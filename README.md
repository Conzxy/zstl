# TinySTL
## Completion
### container
* vector [dynamic array 100%]
* deque [array list 100%]
* list [double cycle linked list 100%]
* forward_list[single linked list 100%](see stl_supplement)
* set [red-black tree 100%]
* map [red-black tree 0%]
* unordered_set[hash table partial]
* unordered_map[hash table partial]
(hash table 暂时没动力写）
* graph[0%]
* skiplist[0%]

* lock-free container which support concurrent

### container adapter
* queue [100%]
* stack [100%]
* priority_queue[i.e. binary heap 100%]

### type support
* type_traits[90% some type traits that user may not be possible to implemete)
* typelist [not STL]

### utility
* pair[partial]
* tuple[partial]
* function[partial]
* iterator[partial?]

### algorithm
* find, find_if, find_if_not
* all_of, any_of, none_of
* count, count_if
* for_each
* mismatch
* search
* find_end
* copy, copy_if
* move, move_if(Not std::move(T&&))
* transform
* generate
* generate_n
* remove, remove_if
* remove_copy, remove_copy_if
* unique
* unique_copy
* replace, replace_if
* replace_copy, replace_copy_if
* swap_ranges
* reverse
* rotate
* lower_bound
* binary_search
...