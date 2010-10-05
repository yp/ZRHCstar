# Yuri Pirola. Copyright (C) 2010. GPLv3.

#
# Project Properties
#
cmake_minimum_required (VERSION 2.8)
project (ZRHCstar)
set (APPLICATION_NAME "Zero-Recombinant Haplotyping with missing genotypes")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2010")
set (APPLICATION_VERSION_MAJOR 1)
set (APPLICATION_VERSION_MINOR 0)
set (APPLICATION_VERSION_PATCH 0)
set (APPLICATION_VERSION_TYPE SNAPSHOT)
set (APPLICATION_VERSION_STRING "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}-${APPLICATION_VERSION_TYPE}")
set (APPLICATION_VENDOR_ID "org.tecnoparco")
set (APPLICATION_VENDOR_NAME "Parco Tecnologico Padano")
set (APPLICATION_VENDOR_URL "www.tecnoparco.org")
set (APPLICATION_ID "${APPLICATION_VENDOR_ID}.${PROJECT_NAME}")


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
set (Boost_ADDITIONAL_VERSIONS "1.39" "1.39.0" "1.40" "1.40.0" "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0" "1.47" "1.47.0" "1.48" "1.48.0" "1.49" "1.49.0" "1.50" "1.50.0")
find_package (Boost COMPONENTS "filesystem" "system" "date_time" REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
find_package (Log4Cxx REQUIRED)
include_directories(${Log4Cxx_INCLUDE_DIRS})
link_directories(${Log4Cxx_LIBRARY_DIRS})


include(ExtractSourceVersionFromGit)

#
# Configure Files
#
file (GLOB_RECURSE CONFIGINPUTS1 include/*.in.h.cmake)
file (GLOB_RECURSE CONFIGINPUTS2 include/*.h.in.cmake)
file (GLOB_RECURSE CONFIGINPUTS3 src/*.in.h.cmake)
file (GLOB_RECURSE CONFIGINPUTS4 src/*.h.in.cmake)
file (GLOB_RECURSE CONFIGINPUTS5 test/*.in.h.cmake)
file (GLOB_RECURSE CONFIGINPUTS6 test/*.h.in.cmake)
list (APPEND CONFIGINPUTS "${CONFIGINPUTS1}")
list (APPEND CONFIGINPUTS "${CONFIGINPUTS2}")
list (APPEND CONFIGINPUTS "${CONFIGINPUTS3}")
list (APPEND CONFIGINPUTS "${CONFIGINPUTS4}")
list (APPEND CONFIGINPUTS "${CONFIGINPUTS5}")
list (APPEND CONFIGINPUTS "${CONFIGINPUTS6}")
foreach (CONFIGINPUT ${CONFIGINPUTS})
    string (REPLACE ".in.h.cmake" ".h" CONFIGOUTPUT1 "${CONFIGINPUT}")
    string (REPLACE ".h.in.cmake" ".h" CONFIGOUTPUT2 "${CONFIGOUTPUT1}")
    configure_file ("${CONFIGINPUT}" "${CONFIGOUTPUT2}")
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
