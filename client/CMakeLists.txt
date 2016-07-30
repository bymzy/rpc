

include_directories(../common/include)
aux_source_directory(. CLIENT_SRCS)
add_executable(client ${CLIENT_SRCS})
target_link_libraries(client libcommon.a)



