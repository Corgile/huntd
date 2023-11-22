if (NOT EXISTS ${VENDOR_PATH}/librdkafka)
    set(BRANCH v2.2.0)
    # 拉取 confluentinc/librdkafka 子模块并切换到指定分支
    message(STATUS "拉取 confluentinc/librdkafka ...")
    execute_process(COMMAND git submodule add -b ${BRANCH} https://github.com/confluentinc/librdkafka.git ${VENDOR_PATH}/librdkafka)
    message(STATUS "confluentinc/librdkafka switched to ${BRANCH}")
endif ()

# 设置是否编译 librdkafka
set(RDKAFKA_BUILD_STATIC ON CACHE BOOL "Build librdkafka")
set(RDKAFKA_BUILD_EXAMPLES OFF CACHE BOOL "Dont Build examples")
add_subdirectory(${VENDOR_PATH}/librdkafka)