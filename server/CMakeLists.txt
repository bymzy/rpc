
include_directories(../common/include)

aux_source_directory(. SERVER_SRCS)
add_executable(server ${SERVER_SRCS})
target_link_libraries(server rt libcommon.a libevent.a pthread)
add_dependencies(server common)



