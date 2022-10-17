FILE(GLOB source_codes ${CMAKE_CURRENT_LIST_DIR}/*.cc)
ADD_LIBRARY(hawking-graph STATIC ${source_codes})

# include
target_include_directories(hawking-graph PRIVATE 
    ${PROTO_OUTPUT_PATH}
    /opt/libs/protobuf/include
    /opt/libs/glog/include
    /opt/libs/gflags/include
    /opt/libs/brpc/include)

# link
target_link_libraries(hawking-graph PRIVATE
    /opt/libs/protobuf/lib/libprotobuf.a
    /opt/libs/brpc/lib/libbrpc.a
    /opt/libs/gflags/lib/libgflags.a
    /opt/libs/glog/lib/libglog.a)

# install
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/dist)
install(TARGETS hawking-graph
        RUNTIME DESTINATION ${INSTALL_DIR}/bin
        ARCHIVE DESTINATION ${INSTALL_DIR}/lib
)

# test
add_subdirectory(test)