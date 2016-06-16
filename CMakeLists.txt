#
# CMakeLists.txt
#
# The MIT License (MIT)
#
# Copyright (c) 2016 Tammo Ippen
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required( VERSION 2.8.12 )
project( timer CXX )

set( TIMER_VERSION_MAJOR 1 )
set( TIMER_VERSION_MINOR 0 )
set( TIMER_VERSION_PATCH 0 )
set( TIMER_VERSION ${TIMER_VERSION_MAJOR}.${TIMER_VERSION_MINOR}.${TIMER_VERSION_PATCH} )

set( enable-timing ON CACHE STRING "En/Disable timing altogether. [default=ON]" )
if ( enable-timing )
  set( ENABLE_TIMING ON )
endif ()

set( enable-scopetimer ON CACHE STRING "En/Disable ScopeTimer. [default=ON]" )
if ( enable-scopetimer )
  set( ENABLE_SCOPETIMER ON )
endif ()

include( GNUInstallDirs )

# RPATH related stuff
# on OS X
set( CMAKE_MACOSX_RPATH ON )
set( CMAKE_SKIP_RPATH FALSE )
set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

find_package( OpenMP )
if ( OPENMP_FOUND )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif ()

include( CheckIncludeFileCXX )
check_include_file_cxx( "algorithm" HAVE_ALGORITHM )
check_include_file_cxx( "cassert" HAVE_CASSERT )
check_include_file_cxx( "cmath" HAVE_CMATH )
check_include_file_cxx( "iomanip" HAVE_IOMANIP )
check_include_file_cxx( "iostream" HAVE_IOSTREAM )
check_include_file_cxx( "map" HAVE_MAP )
check_include_file_cxx( "string" HAVE_STRING )
check_include_file_cxx( "vector" HAVE_VECTOR )

check_include_file_cxx( "stdint.h" HAVE_STDINT_H )
check_include_file_cxx( "sys/time.h" HAVE_SYS_TIME_H )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wformat -Wpointer-arith -Wwrite-strings" )
endif ()

add_subdirectory( src )

find_program( CLANG_FORMAT
    NAMES clang-format clang-format-3.6 clang-format-3.7 clang-format-3.8 clang-format-3.9 )

if ( CLANG_FORMAT STREQUAL "CLANG_FORMAT-NOTFOUND" )
    message( STATUS "No `format` target available as clang-format was not found.")
else ()
    add_custom_target( format
        COMMAND ${CLANG_FORMAT} -i src/include/*.hpp src/*.cpp
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
endif ()

find_program( CPPCHECK cppcheck )

if ( CPPCHECK STREQUAL "CPPCHECK-NOTFOUND" )
    message( STATUS "No `cppcheck` target available as cppcheck was not found.")
else ()
    add_custom_target( cppcheck
        COMMAND ${CPPCHECK} --inconclusive --enable=all -I src/include src
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
endif ()
