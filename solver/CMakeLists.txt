#
# Integrate a SAT solver into the program
#

# We currently support only CryptoMiniSat 2.7.1 by Mate Soos
# http://gitorious.org/cryptominisat

set(SAT_SOLVER "with CryptoMiniSat 2.7.1"
  PARENT_SCOPE)
set(sat_LIBS "cryptominisat"
  PARENT_SCOPE)
set(sat_INCLUDE "cryptominisat"
  PARENT_SCOPE
)
add_subdirectory(cryptominisat)
set(sat_DEFINITIONS "${sat_DEFINITIONS}"
  PARENT_SCOPE)

