if (NOT EXISTS ${VENDOR_PATH}/libpcap)
    set(BRANCH libpcap-1.10.4)
    # 拉取 the-tcpdump-group/libpcap.git 子模块并切换到指定分支
    message(STATUS "拉取 the-tcpdump-group/libpcap.git ...")
    execute_process(COMMAND git submodule add https://github.com/the-tcpdump-group/libpcap.git ${VENDOR_PATH}/libpcap)
    message(STATUS "the-tcpdump-group/libpcap.git has switched to ${BRANCH}")
endif ()
set(MY_PCAP_LIBRARY_NAME hd_pcap)
add_subdirectory(${VENDOR_PATH}/libpcap)