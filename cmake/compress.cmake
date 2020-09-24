# usage:
# compress(
#   HEADERS variable_where_we_save_header_list_add_them_to_the_binary
#   FILES (several) some files to embedd
#   NAMES (several) optional but the names if different from the file
#   SOURCE_GROUP source_group_name_or_it_will_default_to_Generated
# )
#
function(compress)
    # todo: rename to embedd or something better...
    # todo: hrm... should compression be default true or default false

    set(options COMPRESSION)
    set(oneValueArgs SOURCE_GROUP HEADERS)
    set(multiValueArgs FILES NAMES)
    cmake_parse_arguments(COMPRESS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # message(STATUS "compression ${COMPRESS_COMPRESSION}")
    set(compression_argument)
    if(NOT ${COMPRESS_COMPRESSION})
        set(compression_argument "-nocompress")
        set(compression_name "Embedding")
    else()
        set(compression_name "Compressing")
    endif()

    if(NOT ${COMPRESS_SOURCE_GROUP})
        set(COMPRESS_SOURCE_GROUP "Generated")
    endif()

    set(headers_lists)

    foreach(input_name input_file IN ZIP_LISTS COMPRESS_NAMES COMPRESS_FILES)
        if("" STREQUAL "${input_name}")
            get_filename_component(input_name ${input_file} NAME)
        endif()
        get_filename_component(absolute_file ${input_file} ABSOLUTE)

        set(input_variable ${input_name})
        string(TOUPPER "${input_variable}" input_variable)
        string(REPLACE "." "_" input_variable "${input_variable}")

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
            MAIN_DEPENDENCY ${absolute_file}
            COMMAND compress
            ARGS ${compression_argument} "${absolute_file}" "${input_variable}" > "${input_name}.h"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "${compression_name} ${input_variable} to ${input_name}.h"
        )
        set(headers ${headers} ${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h)
    endforeach()

    source_group("${COMPRESS_SOURCE_GROUP}" FILES ${headers})
    set(${COMPRESS_HEADERS} ${headers} PARENT_SCOPE)
endfunction()

function(embed)
    set(options)
    set(oneValueArgs SOURCE_GROUP HEADERS)
    set(multiValueArgs FILES NAMES)
    cmake_parse_arguments(COMPRESS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT ${COMPRESS_SOURCE_GROUP})
        set(COMPRESS_SOURCE_GROUP "Generated")
    endif()

    set(headers_lists)

    foreach(input_name input_file IN ZIP_LISTS COMPRESS_NAMES COMPRESS_FILES)
        if("" STREQUAL "${input_name}")
            get_filename_component(input_name ${input_file} NAME)
        endif()
        get_filename_component(absolute_file ${input_file} ABSOLUTE)

        set(input_variable ${input_name})
        string(TOUPPER "${input_variable}" input_variable)
        string(REPLACE "." "_" input_variable "${input_variable}")

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
            MAIN_DEPENDENCY ${absolute_file}
            COMMAND embed
            ARGS ${compression_argument} "${absolute_file}" "${input_variable}" > "${input_name}.h"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "${compression_name} ${input_variable} to ${input_name}.h"
        )
        set(headers ${headers} ${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h)
    endforeach()

    source_group("${COMPRESS_SOURCE_GROUP}" FILES ${headers})
    set(${COMPRESS_HEADERS} ${headers} PARENT_SCOPE)
endfunction()
