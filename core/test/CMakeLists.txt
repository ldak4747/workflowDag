# compile unitest rpc proto
FILE(GLOB proto_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_proto*.proto)
SET(PROTO_INCLUDE_PATH "/opt/libs/protobuf/include/google/protobuf/")
EXECUTE_PROCESS(COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR} -I=${PROTO_INCLUDE_PATH}
    --cpp_out=${PROTO_OUTPUT_PATH} ${proto_srcs})

FILE(GLOB proto_cc_srcs ${PROTO_OUTPUT_PATH}/*.cc)
FILE(GLOB srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
FILE(GLOB test_op_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_op/*.cc)
add_executable(test ${srcs} ${proto_cc_srcs} ${test_op_srcs})
add_dependencies(test hawking-graph)
set(CMAKE_VERBOSE_MAKEFILE on)

# include
target_include_directories(test PRIVATE
    ${PROTO_OUTPUT_PATH}
    ${CORE_PATH}
    /opt/libs/brpc/include
    /opt/libs/protobuf/include
    /opt/libs/glog/include
    /opt/libs/gflags/include
    /opt/libs/googletest/include)

# link
target_link_libraries(test PRIVATE
    hawking-graph
    -pthread
    /opt/libs/brpc/lib/libbrpc.a
    /opt/libs/gflags/lib/libgflags.a
    /opt/libs/googletest/lib/libgtest.a
    /opt/libs/leveldb/lib/libleveldb.a
    -lssl
    -lcrypto)