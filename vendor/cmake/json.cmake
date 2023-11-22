include(FetchContent)

if (NOT EXISTS ${VENDOR_PATH}/nlohmann-json)
    set(BRANCH v3.11.2)
    # 拉取 nlohmann/json 子模块并切换到指定分支
    message(STATUS "拉取 nlohmann/json ...")
    execute_process(COMMAND git submodule add -b ${BRANCH} https://github.com/nlohmann/json.git ${VENDOR_PATH}/nlohmann-json)
    message(STATUS "nlohmann/json switched to ${BRANCH}")
endif ()

FetchContent_Declare(nlohmann_json SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/nlohmann-json)
FetchContent_MakeAvailable(nlohmann_json)
