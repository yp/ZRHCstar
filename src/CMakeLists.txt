file (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
set (project_MOC_HEADERS app.h)
set (project_LIBS ${Boost_LIBRARIES} ${Log4Cxx_LIBRARIES})
set (project_BIN ${PROJECT_NAME})

add_executable(${project_BIN} ${project_SRCS} ${project_MOC_SRCS_GENERATED})
target_link_libraries(${project_BIN} ${project_LIBS})
set_target_properties(${project_BIN} PROPERTIES VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}" OUTPUT_NAME ${project_BIN} CLEAN_DIRECT_OUTPUT 1)

install(TARGETS ${project_BIN} DESTINATION bin)
