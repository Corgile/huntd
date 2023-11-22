include(FetchContent)

if (NOT EXISTS ${VENDOR_PATH}/dbg-macro)
    set(BRANCH master)
    message(STATUS "拉取 sharkdp/dbg-macro")
    # 拉取 dbg-macro 子模块并切换到指定分支
    execute_process(COMMAND git submodule add https://github.com/sharkdp/dbg-macro.git ${VENDOR_PATH}/dbg-macro)
endif ()

#FetchContent_Declare(dbg_macro GIT_REPOSITORY https://github.com/sharkdp/dbg-macro.git GIT_TAG master)
FetchContent_Declare(hd_dbgmacro SOURCE_DIR ${VENDOR_PATH}/dbg-macro)
FetchContent_MakeAvailable(hd_dbgmacro)
