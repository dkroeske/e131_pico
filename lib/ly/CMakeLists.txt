cmake_minimum_required(VERSION 3.13)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

bison_target(ConfigParser
        ./src/config_parser.y
        ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.c
	COMPILE_FLAGS "--warnings=all --debug --verbose -d"
        DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.h)

flex_target(ConfigScanner
        ./src/config_scanner.l
        ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c
        COMPILE_FLAGS "--header-file=lex.yy.h")

add_flex_bison_dependency(ConfigScanner ConfigParser)

set_source_files_properties(${BISON_ConfigParser_OUTPUTS}
                            ${FLEX_ConfigScanner_OUTPUTS}
                            PROPERTIES
                            COMPILE_FLAGS -I. -Wall -W
                            )

add_library(ly INTERFACE
        ${BISON_ConfigParser_OUTPUTS}
        ${FLEX_ConfigScanner_OUTPUTS} 
)

target_sources(ly INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/lex.yy.c
	${CMAKE_CURRENT_LIST_DIR}/config_parser.c
)

target_include_directories(ly INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)
