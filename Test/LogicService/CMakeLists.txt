
aux_source_directory(. SRCS)
add_executable(test_logic ${SRCS})
add_dependencies(test_logic common)
target_link_libraries(test_logic common)



