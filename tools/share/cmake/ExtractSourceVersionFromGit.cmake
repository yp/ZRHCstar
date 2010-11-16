# - ExtractSourceVersionFromGit
# This module extract the commit-id of the last commit of the current
# git repository. This code sets the following variables:
#  
#  APPLICATION_SOURCE_VERSION:  commit-id of the last commit of the repository
#

#
# Get the source information
#
set (APPLICATION_SOURCE_VERSION "unknown")

find_program (GIT git)
if (${GIT} MATCHES "GIT-NOTFOUND")
message (STATUS "Executable 'git' not found. We now manually look for a git repository...")
get_filename_component(EXP_PATH . REALPATH)
message (STATUS "Does ${EXP_PATH}/.git exist?")
while ((${EXP_PATH} MATCHES "/.") AND (NOT EXISTS "${EXP_PATH}/.git"))
get_filename_component(EXP_PATH ${EXP_PATH} PATH)
message (STATUS "Does ${EXP_PATH}/.git exist?")
endwhile ((${EXP_PATH} MATCHES "/.") AND (NOT EXISTS "${EXP_PATH}/.git"))

set (DOTGIT "${EXP_PATH}/.git")

if (EXISTS "${DOTGIT}/HEAD")

message (STATUS "Found git repository directory ${EXP_PATH}.")
# Searching for the HEAD
file (STRINGS "${DOTGIT}/HEAD" GITHEAD
  LIMIT_COUNT 1
  REGEX "ref:"
)
string (REPLACE "ref: " "" GITFILEHEAD "${GITHEAD}")


execute_process (COMMAND tail -n1 ${DOTGIT}/${GITFILEHEAD}
  COMMAND cut -d\   -f 2
  OUTPUT_VARIABLE APPLICATION_SOURCE_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
else (EXISTS "${DOTGIT}/HEAD")

set (APPLICATION_SOURCE_VERSION "unknown")

endif (EXISTS "${DOTGIT}/HEAD")

else (${GIT} MATCHES "GIT-NOTFOUND")

message (STATUS "Executable 'git' found. Extracting log...")

# Try 'git describe' and fallback to 'git log' if no tags are found

execute_process (COMMAND git describe
  RESULT_VARIABLE describe_ris
  OUTPUT_VARIABLE APPLICATION_SOURCE_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

if (NOT ${describe_ris} EQUAL 0)
execute_process (COMMAND git log -n 1 --format=format:%H
  OUTPUT_VARIABLE APPLICATION_SOURCE_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
endif (NOT ${describe_ris} EQUAL 0)

endif (${GIT} MATCHES "GIT-NOTFOUND")

if (${APPLICATION_SOURCE_VERSION} STREQUAL "unknown")
message (STATUS "!!! The source code is NOT in a git repository.")
else (${APPLICATION_SOURCE_VERSION} STREQUAL "unknown")
message (STATUS "Application source code version: " ${APPLICATION_SOURCE_VERSION})
endif (${APPLICATION_SOURCE_VERSION} STREQUAL "unknown")
