
aux_source_directory(. SRCS)
add_executable(test_driver ${SRCS})
add_dependencies(test_driver common)
target_link_libraries(test_driver common)



