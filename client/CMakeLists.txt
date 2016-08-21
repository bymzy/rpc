

include_directories(../common/include)
aux_source_directory(. CLIENT_SRCS)
aux_source_directory(../common/src COMMON_SERVER_SRCS)
add_executable(client ${CLIENT_SRCS} ${COMMON_SERVER_SRCS})
target_link_libraries(client rt libevent.a pthread)



