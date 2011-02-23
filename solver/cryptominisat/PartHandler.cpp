/*****************************************************************************
CryptoMiniSat -- Copyright (c) 2009 Mate Soos

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "PartHandler.h"
#include "VarReplacer.h"
#include <iostream>
#include <assert.h>
#include <iomanip>

//#define VERBOSE_DEBUG

PartHandler::PartHandler(Solver& s) :
    solver(s)
{
}


const bool PartHandler::handle()
{
    if (solver.doReplace == false)
        return true;

    PartFinder partFinder(solver);
    if (!partFinder.findParts()) {
        #ifdef VERBOSE_DEBUG
        std::cout << "c findParts() found UNSAT. Whole problem is unsat." << std::endl;
        #endif //VERBOSE_DEBUG
        return false;
    }

    uint32_t num_parts = partFinder.getReverseTable().size();
    if (num_parts == 1)
        return true;

    map<uint32_t, vector<Var> > reverseTable = partFinder.getReverseTable();
    assert(num_parts == partFinder.getReverseTable().size());

    vector<pair<uint32_t, uint32_t> > sizes;
    for (map<uint32_t, vector<Var> >::iterator it = reverseTable.begin(); it != reverseTable.end(); it++)
        sizes.push_back(std::make_pair(it->first, (uint32_t)it->second.size()));

    std::sort(sizes.begin(), sizes.end(), sort_pred());
    assert(sizes.size() > 1);

    for (uint32_t it = 0; it < sizes.size()-1; it++) {
        uint32_t part = sizes[it].first;
        vector<Var> vars = reverseTable[part];
        if (solver.verbosity >= 1)
            std::cout << "c Solving part " << part << std::endl;

        Solver newSolver;
        configureNewSolver(newSolver);
        moveVariablesBetweenSolvers(newSolver, vars, part, partFinder);

        assert(solver.varReplacer->getClauses().size() == 0);
        moveClauses(solver.clauses, newSolver, part, partFinder);
        moveClauses(solver.binaryClauses, newSolver, part, partFinder);
        moveClauses(solver.xorclauses, newSolver, part, partFinder);
        moveLearntClauses(solver.binaryClauses, newSolver, part, partFinder);
        moveLearntClauses(solver.learnts, newSolver, part, partFinder);
        assert(checkClauseMovement(newSolver, part, partFinder));

        lbool status = newSolver.solve();
        assert(status != l_Undef);
        if (status == l_False) {
            #ifdef VERBOSE_DEBUG
            std::cout << "c One of the sub-problems was UNSAT. Whole problem is unsat." << std::endl;
            #endif //VERBOSE_DEBUG
            return false;
        }

        //Check that the newly found solution is really unassigned in the
        //original solver
        for (Var var = 0; var < newSolver.nVars(); var++) {
            if (newSolver.model[var] != l_Undef) {
                assert(solver.assigns[var] == l_Undef);
            }
        }

        assert(newSolver.decisionLevel() == 0);
        for (uint32_t i = 0; i < newSolver.trail.size(); i++) {
            solver.uncheckedEnqueue(newSolver.trail[i]);
        }
        solver.ok = (solver.propagate().isNULL());
        assert(solver.ok);

        for (Var var = 0; var < newSolver.nVars(); var++) {
            if (newSolver.model[var] != l_Undef) {
                //Must have been decision var in the old solver!??
                //assert(std::find(decisionVarRemoved.getData(), decisionVarRemoved.getDataEnd(), var) != decisionVarRemoved.getDataEnd());
                assert(savedState[var] == l_Undef);
                assert(partFinder.getVarPart(var) == part);
                if (newSolver.assigns[var] == l_Undef) {
                    savedState[var] = newSolver.model[var];
                }
            }
        }

        if (solver.verbosity >= 1) {
            std::cout << "c Solved part" << std::endl;
            std::cout << "c "
            << "========================================================================================="
            << std::endl;
        }
    }
    if (solver.verbosity >= 1) {
        std::cout << "c Coming back to original instance"
        << std::endl;
        std::cout << "c "
        << "========================================================================================="
        << std::endl;
    }

    solver.order_heap.filter(Solver::VarFilter(solver));

    //Checking that all variables that are not in the remaining part are all
    //non-decision vars, and none have been assigned
    for (Var var = 0; var < solver.nVars(); var++) {
        if (savedState[var] != l_Undef) {
            assert(solver.decision_var[var] == false);
            assert(solver.assigns[var] == l_Undef || solver.level[var] == 0);
        }
    }

    //Checking that all remaining clauses contain only variables that are in the remaining part
    assert(checkClauseMovement(solver, sizes[sizes.size()-1].first, partFinder));

    return true;
}

/**
@brief Sets up the sub-solver with a specific configuration

We need to save on memory, since there might be many subsolvers. We also
need to pass all configurations from the original solver to the sub-solver.
*/
void PartHandler::configureNewSolver(Solver& newSolver) const
{
    newSolver.mtrand.seed(solver.mtrand.randInt());
    newSolver.random_var_freq = solver.random_var_freq;
    newSolver.verbosity = solver.verbosity;
    newSolver.restrictedPickBranch = solver.restrictedPickBranch;
    newSolver.greedyUnbound = solver.greedyUnbound;
    newSolver.findNormalXors = solver.findNormalXors;
    newSolver.findBinaryXors = solver.findBinaryXors;
    newSolver.regFindBinaryXors = solver.regFindBinaryXors;
    newSolver.conglomerateXors = solver.conglomerateXors;
    newSolver.schedSimplification = solver.schedSimplification;
    newSolver.doReplace = solver.doReplace;
    newSolver.failedVarSearch = solver.failedVarSearch;
    newSolver.gaussconfig.dontDisable = solver.gaussconfig.dontDisable;
    newSolver.heuleProcess = solver.heuleProcess;
    newSolver.doSubsumption = solver.doSubsumption;
    newSolver.doPartHandler = solver.doPartHandler;
    newSolver.fixRestartType = solver.fixRestartType;
    newSolver.var_inc = solver.var_inc;
    newSolver.polarity_mode = solver.polarity_mode;

    //Memory-usage reduction
    newSolver.schedSimplification = false;
    newSolver.doSubsumption = false;
    newSolver.doXorSubsumption = false;
    newSolver.doPartHandler = false;
    newSolver.subsWNonExistBins = false;
    newSolver.regSubsWNonExistBins = false;
}

/**
@brief Moves the variables to the new solver, and removes it from the original one

This implies making the right variables decision in the new solver,
and making it non-decision in the old solver.
*/
void PartHandler::moveVariablesBetweenSolvers(Solver& newSolver, vector<Var>& vars, const uint32_t part, const PartFinder& partFinder)
{
    std::sort(vars.begin(), vars.end());
    uint32_t i2 = 0;
    for (Var var = 0; var < solver.nVars(); var++) {
        if (i2 < vars.size() && vars[i2] == var) {
            #ifdef VERBOSE_DEBUG
            if (!solver.decision_var[var]) {
                std::cout << "var " << var + 1 << " is non-decision, but in part... strange." << std::endl;
            }
            #endif //VERBOSE_DEBUG
            newSolver.newVar(solver.decision_var[var]);
            newSolver.activity[var] = solver.activity[var];
            newSolver.order_heap.update(var);
            assert(partFinder.getVarPart(var) == part);
            if (solver.decision_var[var]) {
                solver.setDecisionVar(var, false);
                decisionVarRemoved.push(var);
            }
            i2++;
        } else {
            assert(partFinder.getVarPart(var) != part);
            newSolver.newVar(false);
        }
    }
    solver.order_heap.filter(Solver::VarFilter(solver));
}

/**
@brief Check that when we moved the clauses, we did a good job

I.e. we did not move clauses into a part where they don't belong
*/
const bool PartHandler::checkClauseMovement(const Solver& thisSolver, const uint32_t part, const PartFinder& partFinder) const
{
    if (!checkOnlyThisPart(thisSolver.clauses, part, partFinder))
        return false;
    if (!checkOnlyThisPart(thisSolver.learnts, part, partFinder))
        return false;
    if (!checkOnlyThisPart(thisSolver.binaryClauses, part, partFinder))
        return false;
    if (!checkOnlyThisPart(thisSolver.xorclauses, part, partFinder))
        return false;

    return true;
}

/**
@brief Check that clauses only contain variables belonging to this partFinder

@p cs The clause-list to check
@p part The part that we should check that the clauses belong to
*/
template<class T>
const bool PartHandler::checkOnlyThisPart(const vec<T*>& cs, const uint32_t part, const PartFinder& partFinder) const
{
    for(T * const*it = cs.getData(), * const*end = it + cs.size(); it != end; it++) {
        const T& c = **it;
        for(const Lit *l = c.getData(), *end2 = l + c.size(); l != end2; l++) {
            if (partFinder.getVarPart(l->var()) != part) return false;
        }
    }

    return true;
}

/**
@brief Move clauses from original to the part

The variables that form part of the part will determine if the clause is in the
part or not
*/
void PartHandler::moveClauses(vec<Clause*>& cs, Solver& newSolver, const uint32_t part, PartFinder& partFinder)
{
    Clause **i, **j, **end;
    for (i = j = cs.getData(), j = i , end = i + cs.size(); i != end; i++) {
        if ((**i).learnt() || partFinder.getVarPart((**i)[0].var()) != part) {
            *j++ = *i;
            continue;
        }
        solver.detachClause(**i);
        #ifdef VERBOSE_DEBUG
        std::cout << "clause in this part:"; (**i).plainPrint();
        #endif

        Clause& c = **i;
        vec<Lit> tmp(c.size());
        std::copy(c.getData(), c.getDataEnd(), tmp.getData());
        newSolver.addClause(tmp, c.getGroup());
        //NOTE: we need the CS because otherwise, the addClause could have changed **i, which we need to re-add later!
        clausesRemoved.push(*i);
    }
    cs.shrink(i-j);
}

/**
@brief Move xor clauses from original to the part

The variables that form part of the part will determine if the clause is in the
part or not
*/
void PartHandler::moveClauses(vec<XorClause*>& cs, Solver& newSolver, const uint32_t part, PartFinder& partFinder)
{
    XorClause **i, **j, **end;
    for (i = j = cs.getData(), end = i + cs.size(); i != end; i++) {
        if (partFinder.getVarPart((**i)[0].var()) != part) {
            *j++ = *i;
            continue;
        }
        solver.detachClause(**i);
        #ifdef VERBOSE_DEBUG
        std::cout << "xor clause in this part:"; (**i).plainPrint();
        #endif

        XorClause& c = **i;
        vec<Lit> tmp(c.size());
        std::copy(c.getData(), c.getDataEnd(), tmp.getData());
        newSolver.addXorClause(tmp, c.xorEqualFalse(), c.getGroup());
        //NOTE: we need the CS because otherwise, the addXorClause could have changed **i, which we need to re-add later!
        xorClausesRemoved.push(*i);
    }
    cs.shrink(i-j);
}

/**
@brief Move learnt clauses from original to the part

The variables that form part of the part will determine if the clause is in the
part or not.

Note that learnt clauses might be in both the orignal and the sub-part. In this
case, the learnt clause is deleted, since it unneccesarily connects two
components that otherwise would be distinct
*/
void PartHandler::moveLearntClauses(vec<Clause*>& cs, Solver& newSolver, const uint32_t part, PartFinder& partFinder)
{
    Clause **i, **j, **end;
    for (i = j = cs.getData(), end = i + cs.size() ; i != end; i++) {
        if (!(**i).learnt()) {
            *j++ = *i;
            continue;
        }

        Clause& c = **i;
        assert(c.size() > 0);
        uint32_t clause_part = partFinder.getVarPart(c[0].var());
        bool removed = false;
        for (const Lit* l = c.getData(), *end = l + c.size(); l != end; l++) {
            if (partFinder.getVarPart(l->var()) != clause_part) {
                #ifdef VERBOSE_DEBUG
                std::cout << "Learnt clause in both parts:"; c.plainPrint();
                #endif

                removed = true;
                solver.removeClause(c);
                break;
            }
        }
        if (removed) continue;
        if (clause_part == part) {
            #ifdef VERBOSE_DEBUG
            //std::cout << "Learnt clause in this part:"; c.plainPrint();
            #endif

            solver.detachClause(c);
            newSolver.addLearntClause(c, c.getGlue(), c.getMiniSatAct(), c.getGroup());
            solver.clauseAllocator.clauseFree(&c);
        } else {
            #ifdef VERBOSE_DEBUG
            std::cout << "Learnt clause in other part:"; c.plainPrint();
            #endif

            *j++ = *i;
        }
    }
    cs.shrink(i-j);
}

/**
@brief Adds the saved states to the final solutions

It is only called if the solution of the main part was SAT, and the parts
have all been SAT. We now basically extend the main part with the solutions
of the sub-parts.

Sets all saved states to l_Undef, i.e. unassigns them
*/
void PartHandler::addSavedState()
{
    //Don't add these (non-0-decison-level!) solutions to the 0th decision level
    solver.newDecisionLevel();
    for (Var var = 0; var < savedState.size(); var++) {
        if (savedState[var] != l_Undef) {
            assert(solver.assigns[var] == l_Undef);
            solver.uncheckedEnqueue(Lit(var, savedState[var] == l_False));
            assert(solver.assigns[var] == savedState[var]);
            savedState[var] = l_Undef;
            solver.polarity[var] = (solver.assigns[var] == l_False);
        }
    }

    for (uint32_t i = 0; i < decisionVarRemoved.size(); i++)
        solver.setDecisionVar(decisionVarRemoved[i], true);
    decisionVarRemoved.clear();
}

/**
@brief Re-add the clauses that have been removed because they were in a part

We remove the clauses that are are in a part. Then, at the very end of the
solving, we must re-add them, since if the program is used a library, then
solve() might be called again, at which point we would be missing our clauses!

(Furhtermore, clauses might be added between the end of solving and the
next solve() command, so we cannot keep in memory which parts were what, because
parts might be connected through the added clauses)
*/
void PartHandler::readdRemovedClauses()
{
    FILE* backup_libraryCNFfile = solver.libraryCNFFile;
    solver.libraryCNFFile = NULL;
    for (Clause **it = clausesRemoved.getData(), **end = clausesRemoved.getDataEnd(); it != end; it++) {
        solver.addClause(**it, (*it)->getGroup());
        assert(solver.ok);
    }
    clausesRemoved.clear();

    for (XorClause **it = xorClausesRemoved.getData(), **end = xorClausesRemoved.getDataEnd(); it != end; it++) {
        solver.addXorClause(**it, (**it).xorEqualFalse(), (*it)->getGroup());
        assert(solver.ok);
    }
    xorClausesRemoved.clear();
    solver.libraryCNFFile = backup_libraryCNFfile;
}

