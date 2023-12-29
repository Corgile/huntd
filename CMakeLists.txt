cmake_minimum_required(VERSION 3.25)
project(hound)
set(CMAKE_CXX_STANDARD 20)
set(TARGET hd-benchmark)
#ADD_COMPILE_DEFINITIONS(HD_DEV)
# !>> vendor
set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)
set(CMAKE_MODULE_PATH ${VENDOR_PATH}/cmake;${CMAKE_MODULE_PATH})
include(third-party)
# !<< vendor

#@formatter:off
# all public header for all source files
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/yalanting)
aux_source_directory(source               SOURCE_FILES)
aux_source_directory(source/parser        SOURCE_FILES)
aux_source_directory(source/entity        SOURCE_FILES)
#@formatter:on

add_executable(${TARGET} main.cpp ${SOURCE_FILES}
               include/hound/common/core.hpp)
target_link_libraries(${TARGET} PRIVATE pcap)
target_link_libraries(${TARGET} PRIVATE nlohmann_json)
target_link_libraries(${TARGET} PRIVATE dbg_macro)
