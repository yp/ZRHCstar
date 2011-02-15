find_package (GTest)

if (ENABLE_TESTS)
if (GTEST_FOUND)
if (CMAKE_BUILD_TYPE MATCHES "Debug")
  message (STATUS "Google Test found! Building tests...")

  file (GLOB_RECURSE test_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
  set (test_LIBS ${project_base_LIB} ${sat_LIBS} ${Boost_LIBRARIES} ${Log4Cxx_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
  set (test_BIN test-${PROJECT_NAME})

  add_executable (${test_BIN} ${test_SRCS})
  target_link_libraries (${test_BIN} ${test_LIBS})

  add_custom_target (test ALL
    "${MAINFOLDER}/bin/${test_BIN}"
    DEPENDS ${test_BIN}
    COMMENT "Executing unit tests..."
    VERBATIM SOURCES ${test_SRCS}
  )

else ()

message (STATUS "Tests skipped since we are in a non-debug configuration: CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}' (specify 'Debug' to run the tests)")

endif ()

else (GTEST_FOUND)

  message (STATUS "Google Test *NOT* found! Tests skipped!")

endif (GTEST_FOUND)

else (ENABLE_TESTS)

  message (STATUS "Tests *not* enabled (See ENABLE_TESTS option).")

endif (ENABLE_TESTS)
