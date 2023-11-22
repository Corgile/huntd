cmake_minimum_required(VERSION 3.25)
project(hund)

#if (${BUILD_MODE} STREQUAL "RELEASE_SCOPE")
#    set(TARGET capture-release)
#elseif (${BUILD_MODE} STREQUAL "DEBUG_SCOPE")
#    set(TARGET capture-debug)
#elseif (${BUILD_MODE} STREQUAL "ARG_TEST")
#    set(TARGET capture-test)
#endif ()

set(TARGET huntd)
set(CMAKE_CXX_STANDARD 17)
set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)

message(STATUS ${VENDOR_PATH})

add_compile_definitions(DBG_MACRO_NO_WARNING)
add_compile_definitions(${BUILD_MODE})


# vendor
# 如果vendor文件夹不存在，先创建
if (NOT EXISTS ${VENDOR_PATH})
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${VENDOR_PATH})
endif ()
set(CMAKE_MODULE_PATH ${VENDOR_PATH}/cmake;${CMAKE_MODULE_PATH})

# TODO 根据build类型添加所需依赖
include(json)
include(dbg-macro)
include(kafka)
include(pcap)

# all public header for all source files
include_directories(${PROJECT_SOURCE_DIR}/include)
#@formatter:off
aux_source_directory(source               SOURCE_FILES)
aux_source_directory(source/parser        SOURCE_FILES)
aux_source_directory(source/sink          SOURCE_FILES)
aux_source_directory(source/sink/console  SOURCE_FILES)
aux_source_directory(source/sink/csv      SOURCE_FILES)
aux_source_directory(source/sink/json     SOURCE_FILES)
aux_source_directory(source/sink/kafka    SOURCE_FILES)
#@formatter:on

add_executable(${TARGET} main.cpp ${SOURCE_FILES})
#if (${BUILD_MODE} STREQUAL "RELEASE_SCOPE" OR ${BUILD_MODE} STREQUAL "DEBUG_SCOPE")
target_link_libraries(${TARGET} PRIVATE rdkafka++)
target_link_libraries(${TARGET} PRIVATE pcap)
target_link_libraries(${TARGET} PRIVATE nlohmann_json)
target_link_libraries(${TARGET} PRIVATE dbg_macro)
#endif ()
