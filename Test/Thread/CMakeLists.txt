
aux_source_directory(. SRCS)
add_executable(test_thread ${SRCS})
add_dependencies(test_thread common)
target_link_libraries(test_thread common pthread)



