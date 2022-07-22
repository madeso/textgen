set(src
    src/textgen/textgen.h
    src/textgen/textgen.cc
)
source_group("" FILES ${src})
add_library(textgen_lib STATIC ${src})
target_include_directories(textgen_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(textgen_lib
    PRIVATE
        textgen::project_options
        textgen::project_warnings
)
add_library(textgen::lib ALIAS textgen_lib)
