cmake_minimum_required(VERSION 3.25)
project(hound)
set(CMAKE_CXX_STANDARD 20)
set(TARGET hd)
# kafka
set(INCLUDE_KAFKA OFF)
# !>> vendor
set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)
set(CMAKE_MODULE_PATH ${VENDOR_PATH}/cmake;${CMAKE_MODULE_PATH})
include(third-party)
# !<< vendor

# !>> 处理编译宏
# -DHD_FEATURES:STRING=OFFLINE_MODE,LIVE_MODE,BENCHMARK
string(REGEX MATCHALL [A-Za-z_]+ FEATURES ${HD_FEATURES})
list(SORT result COMPARE STRING ORDER ASCENDING)
foreach (feat ${FEATURES})
    message(STATUS "${BoldWhite}Adding Macro: ${BoldCyan}${feat}${ColorReset}")
    add_compile_definitions(${feat})
    if (${feat} STREQUAL "DEAD_MODE")
        set(TARGET "${TARGET}-dead")
    endif ()
    if (${feat} STREQUAL "LIVE_MODE")
        set(TARGET "${TARGET}-live")
    endif ()
    if (${feat} STREQUAL "BENCHMARK")
        set(TARGET "${TARGET}-bch")
    endif ()
    if (${feat} STREQUAL "SEND_KAFKA")
        set(TARGET "${TARGET}-kfk")
        set(INCLUDE_KAFKA ON)
    endif ()
endforeach ()
message(STATUS "${BoldGreen}Binary Target:  ${TARGET}${ColorReset}")
# !<< 处理编译宏

#@formatter:off
# all public header for all source files
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/yalanting)
aux_source_directory(source               SOURCE_FILES)
aux_source_directory(source/parser        SOURCE_FILES)
aux_source_directory(source/entity        SOURCE_FILES)
#@formatter:on

add_executable(${TARGET} main.cpp ${SOURCE_FILES})
if (INCLUDE_KAFKA)
    message(STATUS "${BoldGreen}With librdkafka${ColorReset}")
    target_link_libraries(${TARGET} PRIVATE rdkafka rdkafka++)
endif ()
target_link_libraries(${TARGET} PRIVATE pcap)
target_link_libraries(${TARGET} PRIVATE nlohmann_json)
target_link_libraries(${TARGET} PRIVATE dbg_macro)
