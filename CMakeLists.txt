cmake_minimum_required (VERSION 2.6)
project( cscript C )

# written by: Oliver Cordes 2017-07-20
# changed by: Oliver Cordes 2017-08-13

set(VERSION 0.1.2)

# get current date
execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE BUILD)
string( REPLACE "\n" "" BUILD ${BUILD})
message(STATUS "Compilation date = ${BUILD}")

if(UNIX AND NOT APPLE)
   set(LINUX TRUE)
endif()


# add local cmake modules
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

# define the target and parameter

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `sources`

file(GLOB_RECURSE sources src/*.c src/*.h)

IF(CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-Wall -Wextra -Wshadow)
ENDIF()



add_executable( cscript ${sources} )

# just for example add some compiler flags
target_compile_options( cscript  PUBLIC  -g -O3 -Wall )



# this lets me include files relative to the root src dir with a <> pair
target_include_directories( cscript PUBLIC src )


# add libraries

if(LINUX)
   #message(STATUS ">>> Linux")
   find_package(OpenSSL REQUIRED)
   if( OpenSSL_FOUND )
      include_directories(${OPENSSL_INCLUDE_DIRS})
       link_directories(${OPENSSL_LIBRARIES})
       message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
   else()
       # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
   endif(OpenSSL_FOUND)
   target_link_libraries( cscript ${OPENSSL_LIBRARIES})
endif()

# add the math lib
target_link_libraries( cscript m )


# os configuration file
INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
#CHECK_INCLUDE_FILES (utime.h HAVE_UTIME_H)
CHECK_INCLUDE_FILES (sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES (sys/resource.h HAVE_SYS_RESOURCE_H)
configure_file ("${PROJECT_SOURCE_DIR}/src/config.h.in"
                "${PROJECT_BINARY_DIR}/config.h" )
# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})


# sphinx part
find_package( Sphinx )

if ( Sphinx_FOUND )

if(NOT DEFINED SPHINX_THEME)
    set(SPHINX_THEME default)
endif()

if(NOT DEFINED SPHINX_THEME_DIR)
    set(SPHINX_THEME_DIR)
endif()

# configured documentation tools and intermediate build results
set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")

# Sphinx cache with pickled ReST documents
set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")

file(GLOB_RECURSE DOC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/doc/source/* )

# HTML output directory
set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/source/conf.py.in"
    "${BINARY_BUILD_DIR}/conf.py"
    @ONLY)

add_custom_command(
    OUTPUT ${SPHINX_HTML_DIR}/index.html
    DEPENDS ${DOC_SOURCES}
    COMMAND ${SPHINX_EXECUTABLE}
        -q -b html
        -c "${BINARY_BUILD_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/doc/source"
        "${SPHINX_HTML_DIR}"
    COMMENT "Building HTML documentation with Sphinx")

add_custom_target(docs_html
    DEPENDS ${SPHINX_HTML_DIR}/index.html
    )

add_custom_command(
    OUTPUT ${SPHINX_PDF_DIR}/cscript.pdf
    DEPENDS ${DOC_SOURCES}
    COMMAND ${SPHINX_EXECUTABLE}
            -q -b latex
            -c "${BINARY_BUILD_DIR}"
            -d "${SPHINX_CACHE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/doc/source"
            "${SPHINX_PDF_DIR}"
    COMMAND make -C ${SPHINX_PDF_DIR} all-pdf
    COMMENT "Building PDF documentation with Sphinx")

add_custom_target(docs_pdf
    DEPENDS ${SPHINX_PDF_DIR}/cscript.pdf
    )

endif()


# install part
install(TARGETS cscript DESTINATION bin)

## CPack part

## CPack version numbers for release tarball name.
#set(CPACK_PACKAGE_VERSION_MAJOR 0)
#set(CPACK_PACKAGE_VERSION_MINOR 0)
#set(CPACK_PACKAGE_VERSION_PATCH 1)

#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Algorithm foR Charge Transfer Inefficency Correcton - arctic")
#set(CPACK_PACKAGE_VENDOR "Oliver Cordes/Ole Marggraf")
#set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README)
#set(CPACK_GENERATOR TGZ)
#set(
#CPACK_SOURCE_PACKAGE_FILE_NAME "arctic-${VERSION}"
#CACHE INTERNAL "tarball basename"
#)
#set(CPACK_SOURCE_GENERATOR TGZ)
## The following components are regex's to match anywhere (unless anchored)
## in absolute path + filename to find files or directories to be excluded
## from source tarball.
#set(CPACK_SOURCE_IGNORE_FILES
#"~$"
#"\\\\.cvsignore$"
#"^${PROJECT_SOURCE_DIR}.*/CVS/"
#"^${PROJECT_SOURCE_DIR}/debian/"
#"^${PROJECT_SOURCE_DIR}/build/"
#"^${PROJECT_SOURCE_DIR}/demo/"
#"^${PROJECT_SOURCE_DIR}/test_suite/data/*.fitsq"
#"^${PROJECT_SOURCE_DIR}/tests/"
#)
##message("CPACK_SOURCE_IGNORE_FILES = ${CPACK_SOURCE_IGNORE_FILES}")
#include(CPack)
