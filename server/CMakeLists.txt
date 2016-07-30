
include_directories(../common/include)

aux_source_directory(. SERVER_SRCS)
aux_source_directory(../common/src COMMON_SERVER_SRCS)

add_executable(server ${SERVER_SRCS} ${COMMON_SERVER_SRCS})
target_link_libraries(server rt libevent.a pthread)
add_dependencies(server common)



