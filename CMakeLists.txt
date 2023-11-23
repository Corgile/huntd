cmake_minimum_required(VERSION 3.25)
project(hound)
set(CMAKE_CXX_STANDARD 17)
set(TARGET hd)
# kafka
set(SEND_KAFKA CACHE BOOL "是否包含Kafka功能" OFF)

# -DBUILD_MODE:STRING=OFFLINE_MODE,LIVE_MODE,BENCHMARK
string(REGEX MATCHALL "[A-Z_]+|[a-z_]+" result ${BUILD_MODE})
list(SORT result COMPARE STRING ORDER ASCENDING)

foreach (macro ${result})
    message(STATUS ${macro})
    add_compile_definitions(${macro})
    if (${macro} STREQUAL "DEAD_MODE")
        set(TARGET "${TARGET}-dead")
    endif ()
    if (${macro} STREQUAL "LIVE_MODE")
        set(TARGET "${TARGET}-live")
    endif ()
    if (${macro} STREQUAL "BENCHMARK")
        set(TARGET "${TARGET}-bch")
    endif ()
    if (${macro} STREQUAL "INCLUDE_KAFKA")
        set(TARGET "${TARGET}-kfk")
        set(SEND_KAFKA ON)
    endif ()
endforeach ()

message(STATUS "Build Target:  ${TARGET}")

add_compile_definitions(DBG_MACRO_NO_WARNING)

# vendor
set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)
# 如果vendor文件夹不存在，先创建
if (NOT EXISTS ${VENDOR_PATH})
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${VENDOR_PATH})
endif ()
set(CMAKE_MODULE_PATH ${VENDOR_PATH}/cmake;${CMAKE_MODULE_PATH})

# TODO 根据build类型添加所需依赖
include(json)
include(dbg-macro)
if (SEND_KAFKA)
    include(kafka)
endif ()
include(pcap)

# all public header for all source files
include_directories(${PROJECT_SOURCE_DIR}/include)
#@formatter:off
aux_source_directory(source               SOURCE_FILES)
aux_source_directory(source/parser        SOURCE_FILES)
aux_source_directory(source/entity        SOURCE_FILES)
aux_source_directory(source/sink          SOURCE_FILES)
aux_source_directory(source/sink/console  SOURCE_FILES)
aux_source_directory(source/sink/csv      SOURCE_FILES)
aux_source_directory(source/sink/json     SOURCE_FILES)
aux_source_directory(source/sink/kafka    SOURCE_FILES)
#@formatter:on

add_executable(${TARGET} main.cpp ${SOURCE_FILES})
#if (${BUILD_MODE} STREQUAL "RELEASE_SCOPE" OR ${BUILD_MODE} STREQUAL "DEBUG_SCOPE")
if (SEND_KAFKA)
    target_link_libraries(${TARGET} PRIVATE rdkafka++)
endif ()
target_link_libraries(${TARGET} PRIVATE pcap)
target_link_libraries(${TARGET} PRIVATE nlohmann_json)
target_link_libraries(${TARGET} PRIVATE dbg_macro)
#endif ()
