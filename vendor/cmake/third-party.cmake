
add_compile_definitions(DBG_MACRO_NO_WARNING)

# TODO 根据build类型添加所需依赖
include(ansi-color)
include(json)
include(dbg-macro)
if (SEND_KAFKA)
    include(kafka)
endif ()
include(pcap)
