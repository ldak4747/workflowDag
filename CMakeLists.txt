project(hawking_graph)
cmake_minimum_required(VERSION 3.10)

# 1. set compiler and compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall -g -ggdb")

# 2. service_configuration proto compile
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/proto)
set(PROTO_INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/configuration)
set(PROTO_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/proto)
EXECUTE_PROCESS(COMMAND protoc -I=${PROTO_INPUT_PATH} --cpp_out=${PROTO_OUTPUT_PATH} 
                ${PROTO_INPUT_PATH}/service_configuration.proto)

# 3. compile
set(CORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(core)