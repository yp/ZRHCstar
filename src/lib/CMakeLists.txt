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


add_library(${project_base_LIB} STATIC
  ${project_SRCS} ${project_MOC_SRCS_GENERATED}
)


set_target_properties(${project_base_LIB}
  PROPERTIES
  COMPILE_DEFINITIONS_RELEASE NO_DBG_ASSERTIONS
)
