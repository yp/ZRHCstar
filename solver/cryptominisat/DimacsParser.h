#ifndef DIMACSPARSER_H
#define DIMACSPARSER_H

#include <string>
#include "SolverTypes.h"
#include "constants.h"
#include "StreamBuffer.h"
#include "Vec.h"

#ifdef _MSC_VER
#include <msvc/stdint.h>
#else
#include <stdint.h>
#endif //_MSC_VER

#ifndef DISABLE_ZLIB
#include <zlib.h>
#endif // DISABLE_ZLIB

#ifdef STATS_NEEDED
#include "Logger.h"
#endif //STATS_NEEDED

class Solver;

/**
@brief Parses up a DIMACS file that my be zipped
*/
class DimacsParser
{
    public:
        DimacsParser(Solver* solver, const bool debugLib, const bool debugNewVar, const bool grouping, const bool addAsLearnt = false);

        #ifdef DISABLE_ZLIB
        void parse_DIMACS(FILE * input_stream);
        #else
        void parse_DIMACS(gzFile input_stream);
        #endif // DISABLE_ZLIB

    private:
        void parse_DIMACS_main(StreamBuffer& in);
        void skipWhitespace(StreamBuffer& in);
        void skipLine(StreamBuffer& in);
        std::string untilEnd(StreamBuffer& in);
        int parseInt(StreamBuffer& in, uint32_t& len);
        float parseFloat(StreamBuffer& in);
        void parseString(StreamBuffer& in, std::string& str);
        void readClause(StreamBuffer& in, vec<Lit>& lits);
        void parseClauseParameters(StreamBuffer& in, bool& learnt, uint32_t& glue, float& miniSatAct);
        void readFullClause(StreamBuffer& in);
        bool match(StreamBuffer& in, const char* str);
        void printHeader(StreamBuffer& in);
        void parseComments(StreamBuffer& in, const std::string str);
        std::string stringify(uint32_t x);


        Solver *solver;
        const bool debugLib;
        const bool debugNewVar;
        const bool grouping;
        const bool addAsLearnt;

        uint32_t debugLibPart; ///<printing partial solutions to debugLibPart1..N.output when "debugLib" is set to TRUE
        uint32_t groupId;
        vec<Lit> lits; ///<To reduce temporary creation overhead
        uint32_t numLearntClauses; ///<Number of learnt non-xor clauses added
        uint32_t numNormClauses; ///<Number of non-learnt, non-xor claues added
        uint32_t numXorClauses; ///<Number of non-learnt xor clauses added
};

#endif //DIMACSPARSER_H
