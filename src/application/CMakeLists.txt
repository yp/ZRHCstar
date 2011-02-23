# Set the project sources to all the source files in the current directory
file (GLOB_RECURSE project_SRCS
  [A-Za-z0-9]*.cpp
  [A-Za-z0-9]*.cxx
  [A-Za-z0-9]*.cc
  [A-Za-z0-9]*.C
  [A-Za-z0-9]*.c
  [A-Za-z0-9]*.h
  [A-Za-z0-9]*.hpp
)

set (project_BIN ${PROJECT_NAME})

set (project_LIBS
  ${sat_LIBS}
  ${Boost_LIBRARIES}
  ${Log4Cxx_LIBRARIES}
  ${project_base_LIB}
)

add_executable(${project_BIN}
  ${project_SRCS}
  ${project_MOC_SRCS_GENERATED}
)
target_link_libraries(${project_BIN} ${project_LIBS})
set_target_properties(
  ${project_BIN}
  PROPERTIES
  VERSION "${APPLICATION_VERSION_STRING}"
  OUTPUT_NAME ${project_BIN}
  CLEAN_DIRECT_OUTPUT 1
  COMPILE_DEFINITIONS_RELEASE NO_DBG_ASSERTIONS
)

install(TARGETS ${project_BIN} DESTINATION bin)
