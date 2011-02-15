message("   Building CryptoMiniSat as SAT solver.")

if(Boost_FOUND)
    message("   CryptoMiniSat: Boost found --- clauses will be packed (speedup)")
    add_definitions(-DUSE_POOLS)
else(Boost_FOUND)
    message("   CryptoMiniSat: Boost NOT found --- clauses will NOT be packed (slowdown)")
endif(Boost_FOUND)

add_definitions(-DUSE_GAUSS)
add_definitions(-DDISABLE_ZLIB)

get_property(DEFINITIONS
  DIRECTORY
  PROPERTY COMPILE_DEFINITIONS)
set(sat_DEFINITIONS "${DEFINITIONS}"
  PARENT_SCOPE)
unset(DEFINITIONS)

add_library(cryptominisat STATIC
    Logger.cpp
    Solver.cpp
    SolverDebug.cpp
    SolverMisc.cpp
    Gaussian.cpp
    PackedRow.cpp
    XorFinder.cpp
    MatrixFinder.cpp
    VarReplacer.cpp
    FindUndef.cpp
    ClauseCleaner.cpp
    RestartTypeChooser.cpp
    FailedVarSearcher.cpp
    PartFinder.cpp
    Subsumer.cpp
    PartHandler.cpp
    XorSubsumer.cpp
    StateSaver.cpp
    ClauseAllocator.cpp
    UselessBinRemover.cpp
    OnlyNonLearntBins.cpp
    CompleteDetachReattacher.cpp
    DimacsParser.cpp
)


