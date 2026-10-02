cmake_minimum_required( VERSION 3.10.0 )

#Find clang-format binary
find_program(CLF_BIN clang-format DOC "Path to clang-format binary")
if(CLF_BIN)
    set(SRC_LIST ${alsavolume_SRCS} ${alsavolume_HDRS})
    add_custom_target(fix-codestyle
        COMMAND ${CLF_BIN}
        ARGS
        --verbose
        -style=file
        -i ${SRC_LIST}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Fix codestyle with clang-format"
        VERBATIM
    )
endif()
