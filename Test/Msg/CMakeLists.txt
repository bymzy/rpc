
aux_source_directory(. SRCS)
add_executable(testmsg ${SRCS})
add_dependencies(testmsg comon)
target_link_libraries(testmsg common)



