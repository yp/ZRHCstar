# Yuri Pirola. Copyright (C) 2010. GPLv3.

#
# Project Properties
#
cmake_minimum_required (VERSION 2.8)
project (ZRHCstar)

set (APPLICATION_NAME "Zero-Recombinant Haplotyping with missing genotypes")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2010")
set (APPLICATION_VENDOR_ID "org.tecnoparco")
set (APPLICATION_VENDOR_NAME "Parco Tecnologico Padano")
set (APPLICATION_VENDOR_URL "www.tecnoparco.org")
set (APPLICATION_ID "${APPLICATION_VENDOR_ID}.${PROJECT_NAME}")

include(CMakeOptions.txt)

#
# Debugging Options
#
set (CMAKE_VERBOSE_MAKEFILE 0) # Use 1 for debugging, 0 for release

#
# Project Output Paths
#
set (MAINFOLDER ${PROJECT_SOURCE_DIR})
set (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")
set (LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib")

set (project_base_LIB ${PROJECT_NAME}-lib)

#
# Project Search Paths
#
list (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
list (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/tools")
list (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/thirdparty")
set (CMAKE_MODULE_PATH "${MAINFOLDER}/tools/share/cmake")
include_directories("${MAINFOLDER}/include")

#
# Locate Project Prerequisites
#
set (Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0")
find_package (Boost COMPONENTS "filesystem" "system" "date_time" "program_options" "iostreams" REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
find_package (Log4Cxx REQUIRED)
include_directories(${Log4Cxx_INCLUDE_DIRS})
link_directories(${Log4Cxx_LIBRARY_DIRS})

set (CMAKE_CXX_FLAGS "-std=c++0x -Wall ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "-mtune=native -fomit-frame-pointer ${CMAKE_CXX_FLAGS_RELEASE}")

include(ExtractSourceVersionFromGit)
set (APPLICATION_VERSION_STRING "${APPLICATION_SOURCE_VERSION}") # See the version from the 'git' repositories

#
# Configure Files
#
file (GLOB_RECURSE CONFIGINPUTS include/*.h.in.cmake)
foreach (CONFIGINPUT ${CONFIGINPUTS})
    string (REPLACE ".h.in.cmake" ".h" CONFIGOUTPUT "${CONFIGINPUT}")
    configure_file ("${CONFIGINPUT}" "${CONFIGOUTPUT}")
endforeach (CONFIGINPUT ${CONFIGINPUTS})

#
# Add Build Targets
#
add_subdirectory(src)
add_subdirectory(test)

#
# Add Install Targets
#
if (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")
    install(DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}" DESTINATION "include")
endif (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")

#
# Add Documentation Targets
#
include (DocumentationTargets)
