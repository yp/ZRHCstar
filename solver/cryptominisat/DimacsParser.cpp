/******************************************************************************************[Main.C]
MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
CryptoMiniSat -- Copyright (c) 2009 Mate Soos

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include "DimacsParser.h"
#include <sstream>
#include <iostream>
#include <iomanip>

#include "Solver.h"

#ifdef VERBOSE_DEBUG
#define DEBUG_COMMENT_PARSING
#endif //VERBOSE_DEBUG

//#define DEBUG_COMMENT_PARSING

DimacsParser::DimacsParser(Solver* _solver, const bool _debugLib, const bool _debugNewVar, const bool _grouping, const bool _addAsLearnt):
    solver(_solver)
    , debugLib(_debugLib)
    , debugNewVar(_debugNewVar)
    , grouping(_grouping)
    , addAsLearnt(_addAsLearnt)
    , groupId(0)
{}

/**
@brief Skips all whitespaces
*/
void DimacsParser::skipWhitespace(StreamBuffer& in)
{
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in;
}

/**
@brief Skips until the end of the line
*/
void DimacsParser::skipLine(StreamBuffer& in)
{
    for (;;) {
        if (*in == EOF || *in == '\0') return;
        if (*in == '\n') {
            ++in;
            return;
        }
        ++in;
    }
}

/**
@brief Returns line until the end of line
*/
std::string DimacsParser::untilEnd(StreamBuffer& in)
{
    std::string ret;

    while(*in != EOF && *in != '\0' && *in != '\n') {
        ret += *in;
        ++in;
    }

    return ret;
}

/**
@brief Parses in an integer
*/
int DimacsParser::parseInt(StreamBuffer& in, uint32_t& lenParsed)
{
    lenParsed = 0;
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9') {
        lenParsed++;
        val = val*10 + (*in - '0'),
              ++in;
    }
    return neg ? -val : val;
}

float DimacsParser::parseFloat(StreamBuffer& in)
{
    uint32_t len;
    uint32_t main = parseInt(in, len);
    if (*in != '.') {
        printf("PARSE ERROR! Float does not contain a dot! Instead it contains: %c\n", *in);
        exit(3);
    }
    ++in;
    uint32_t sub = parseInt(in, len);

    uint32_t exp = 1;
    for (uint32_t i = 0;i < len; i++) exp *= 10;
    return (float)main + ((float)sub/exp);
}


std::string DimacsParser::stringify(uint32_t x)
{
    std::ostringstream o;
    o << x;
    return o.str();
}

/**
@brief Parse a continious set of characters from "in" to "str".

\todo EOF is not checked for!!
*/
void DimacsParser::parseString(StreamBuffer& in, std::string& str)
{
    str.clear();
    skipWhitespace(in);
    while (*in != ' ' && *in != '\n') {
        str += *in;
        ++in;
    }
}

/**
@brief Reads in a clause and puts it in lit
@p[out] lits
*/
void DimacsParser::readClause(StreamBuffer& in, vec<Lit>& lits)
{
    int     parsed_lit;
    Var     var;
    uint32_t len;
    lits.clear();
    for (;;) {
        parsed_lit = parseInt(in, len);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        if (!debugNewVar) {
            while (var >= solver->nVars()) solver->newVar();
        }
        lits.push( (parsed_lit > 0) ? Lit(var, false) : Lit(var, true) );
    }
}

/**
@brief Matches parameter "str" to content in "in"
*/
bool DimacsParser::match(StreamBuffer& in, const char* str)
{
    for (; *str != 0; ++str, ++in)
        if (*str != *in)
            return false;
    return true;
}

/**
@brief Prints the data in "p cnf VARS CLAUSES" header in DIMACS

We don't actually do \b anything with these. It's just printed for user
happyness. However, I think it's useless to print it, since it might mislead
users to think that their headers are correct, even though a lot of headers are
completely wrong, thanks to MiniSat printing the header, but not checking it.
Not checking it is \b not a problem. The problem is printing it such that
people believe it's validated
*/
void DimacsParser::printHeader(StreamBuffer& in)
{
    uint32_t len;

    if (match(in, "p cnf")) {
        int vars    = parseInt(in, len);
        int clauses = parseInt(in, len);
        if (solver->verbosity >= 1) {
            std::cout << "c -- header says num vars:   " << std::setw(12) << vars << std::endl;
            std::cout << "c -- header says num clauses:" <<  std::setw(12) << clauses << std::endl;
        }
    } else {
        printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    }
}

/**
@brief Parse up comment lines which could contain important information

In CryptoMiniSat we save quite a bit of information in the comment lines.
These need to be parsed up. This function achieves that. Informations that
can be given:
\li "c Solver::newVar() called" -- we execute Solver::newVar()
\li "c Solver::solve() called" -- we execute Solver::solve() and dump the
solution to debugLibPartX.out, where X is a number that starts with 1 and
increases to N, where N is the number of solve() instructions
\li variable names in the form of "c var VARNUM NAME"
*/
void DimacsParser::parseComments(StreamBuffer& in, const std::string str)
{
    uint32_t len;
    #ifdef DEBUG_COMMENT_PARSING
    std::cout << "Parsing comments" << std::endl;
    #endif //DEBUG_COMMENT_PARSING

    if (str == "v" || str == "var") {
        int var = parseInt(in, len);
        skipWhitespace(in);
        if (var <= 0) std::cout << "PARSE ERROR! Var number must be a positive integer" << std::endl, exit(3);
        std::string name = untilEnd(in);
        solver->setVariableName(var-1, name.c_str());

        #ifdef DEBUG_COMMENT_PARSING
        std::cout << "Parsed 'c var'" << std::endl;
        #endif //DEBUG_COMMENT_PARSING
    } else if (debugLib && str == "Solver::solve()") {
        lbool ret = solver->solve();
        std::string s = "debugLibPart" + stringify(debugLibPart) +".output";
        FILE* res = fopen(s.c_str(), "w");
        if (ret == l_True) {
            fprintf(res, "SAT\n");
            for (Var i = 0; i != solver->nVars(); i++) {
                if (solver->model[i] != l_Undef)
                    fprintf(res, "%s%d ", (solver->model[i]==l_True)?"":"-", i+1);
            }
            fprintf(res, "0\n");
        } else if (ret == l_False) {
            fprintf(res, "UNSAT\n");
        } else if (ret == l_Undef) {
            assert(false);
        } else {
            assert(false);
        }
        fclose(res);
        debugLibPart++;

        #ifdef DEBUG_COMMENT_PARSING
        std::cout << "Parsed Solver::solve()" << std::endl;
        #endif //DEBUG_COMMENT_PARSING
    } else if (debugNewVar && str == "Solver::newVar()") {
        solver->newVar();

        #ifdef DEBUG_COMMENT_PARSING
        std::cout << "Parsed Solver::newVar()" << std::endl;
        #endif //DEBUG_COMMENT_PARSING
    } else {
        #ifdef DEBUG_COMMENT_PARSING
        std::cout << "didn't understand in CNF file: 'c " << str << std::endl;
        #endif //DEBUG_COMMENT_PARSING
    }
    skipLine(in);
}

/**
@brief Parses clause parameters given as e.g. "c clause learnt yes glue 4 miniSatAct 5.2"
*/
void DimacsParser::parseClauseParameters(StreamBuffer& in, bool& learnt, uint32_t& glue, float& miniSatAct)
{
    std::string str;
    uint32_t len;

    //Parse in if we are a learnt clause or not
    ++in;
    parseString(in, str);
    //std::cout << str<< std::endl;
    if (str != "learnt") goto addTheClause;

    ++in;
    parseString(in, str);
    //std::cout << str<< std::endl;
    if (str == "yes") learnt = true;
    else if (str == "no") {
        learnt = false;
        goto addTheClause;
    }
    else {
        std::cout << "parsed in instead of yes/no: '" << str << "'" << std::endl;
        goto addTheClause;
        //std::cout << "PARSE ERROR: line contains \"c learnt\" but is not followed by yes/no" << std::endl;
        //exit(-1);
    }
    //std::std::cout << "Learnt? " << learnt << std::endl;

    //Parse in Glue value
    ++in;
    parseString(in, str);
    if (str != "glue") goto addTheClause;
    //std::std::cout << "read GLUE" << std::endl;
    ++in;
    glue = parseInt(in, len);
    //std::cout << "glue: " << glue << std::endl;

    //Parse in MiniSat activity
    ++in;
    parseString(in, str);
    if (str != "miniSatAct") goto addTheClause;
    //std::cout << "read MINISATACT" << std::endl;
    ++in;
    miniSatAct = parseFloat(in);
    //std::cout << "MiniSatAct:" << miniSatAct << std::endl;

    addTheClause:
    skipLine(in);
    return;
}

/**
@brief Parses in a clause and its optional attributes

We might have lines like:
\li "c clause learnt yes glue 4 miniSatAct 5.2" which we need to parse up and
make the clause learnt.
\li Also, groupings can be given with "c group NUM NAME" after the clause.
\li Furthermore, we need to take care, since comments might mean orders like
"c Solver::newVar() called", which needs to be parsed with parseComments()
-- this, we delegate
*/
void DimacsParser::readFullClause(StreamBuffer& in)
{
    bool xor_clause = false;
    bool learnt = false;
    uint32_t glue = 100.0;
    float miniSatAct = 10.0;
    std::string name;
    std::string str;
    uint32_t len;
    bool needToParseComments = false;

    //read in the actual clause
    if ( *in == 'x') xor_clause = true, ++in;
    readClause(in, lits);
    skipLine(in);

    //now read in grouping information, etc.
    if (!grouping) groupId++;
    else {
        if (*in != 'c') {
            std::cout << "PARSE ERROR! Group must be present after earch clause ('c' missing after clause line)" << std::endl;
            exit(3);
        }
        ++in;

        parseString(in, str);
        if (str != "g" && str != "group") {
            std::cout << "PARSE ERROR! Group must be present after each clause('group' missing)!" << std::endl;
            std::cout << "Instead of 'group' there was:" << str << std::endl;
            exit(3);
        }

        groupId = parseInt(in, len);
        if (groupId <= 0) printf("PARSE ERROR! Group number must be a positive integer\n"), exit(3);

        skipWhitespace(in);
        name = untilEnd(in);
    }

    //Parse comments or parse clause type (learnt, glue value, etc.)
    if (*in == 'c') {
        ++in;
        parseString(in, str);
        if (str == "clause") {
            parseClauseParameters(in, learnt, glue, miniSatAct);
        } else {
            needToParseComments = true;
        }
    }

    if (xor_clause) {
        bool xorEqualFalse = false;
        for (uint32_t i = 0; i < lits.size(); i++)
            xorEqualFalse ^= lits[i].sign();

        solver->addXorClause(lits, xorEqualFalse, groupId, name.c_str());
        numXorClauses++;
    } else {
        if (addAsLearnt || learnt) {
            solver->addLearntClause(lits, glue, miniSatAct, groupId);
            numLearntClauses++;
        } else {
            solver->addClause(lits, groupId, name.c_str());
            numNormClauses++;
        }
    }

    if (needToParseComments) {
        #ifdef DEBUG_COMMENT_PARSING
        std::cout << "Need to parse comments:" << str << std::endl;
        #endif //DEBUG_COMMENT_PARSING
        parseComments(in, str);
    }
}

/**
@brief The main function: parses in a full DIMACS file

Parses in header, the clauses, and special comment lines that define clause
groups, clause group names, and variable names, plus it parses up special
comments that have to do with debugging Solver::newVar() and Solver::solve()
calls for library-debugging
*/
void DimacsParser::parse_DIMACS_main(StreamBuffer& in)
{
    std::string str;

    for (;;) {
        skipWhitespace(in);
        switch (*in) {
        case EOF:
            return;
        case 'p':
            printHeader(in);
            break;
        case 'c':
            ++in;
            parseString(in, str);
            parseComments(in, str);
            break;
        default:
            readFullClause(in);
            break;
        }
    }
}

#ifdef DISABLE_ZLIB
void DimacsParser::parse_DIMACS(FILE * input_stream)
#else
void DimacsParser::parse_DIMACS(gzFile input_stream)
#endif // DISABLE_ZLIB
{
    debugLibPart = 1;
    numLearntClauses = 0;
    numNormClauses = 0;
    numXorClauses = 0;
    uint32_t origNumVars = solver->nVars();

    StreamBuffer in(input_stream);
    parse_DIMACS_main(in);

    std::cout << "c -- clauses added: "
    << std::setw(12) << numLearntClauses
    << " learnts, "
    << std::setw(12) << numNormClauses
    << " normals, "
    << std::setw(12) << numXorClauses
    << " xors"
    << std::endl;

    std::cout << "c -- vars added " << std::setw(10) << (solver->nVars() - origNumVars)
    << std::endl;
}
