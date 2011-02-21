/**************************************************************************
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
*****************************************************************************/

#include "Solver.h"

/**
@brief Helper class to completely detaches all(or only non-native) clauses, and then re-attach all

Used in classes that (may) do a lot of clause-changning, in which case
detaching&reattaching of clauses would be neccessary to do
individually, which is \b very slow

A main use-case is the following:
-# detach all clauses
-# play around with all clauses as desired. Cannot call solver.propagate() here
-# attach all clauses again

A somewhat more complicated, but more interesting use-case is the following:
-# detach only non-natively stored clauses from watchlists
-# play around wil all clauses as desired. 2- and 3-long clauses can still
be propagated with solver.propagate() -- this is quite a nice trick, in fact
-# detach all clauses (i.e. also native ones)
-# attach all clauses
*/
class CompleteDetachReatacher
{
    public:
        CompleteDetachReatacher(Solver& solver);
        const bool completelyReattach();
        void completelyDetach();
        void detachPointerUsingClauses();

    private:
        void clearWatchOfPointerUsers(vec<Watched>& ws);

        void cleanAndAttachClauses(vec<Clause*>& cs, const bool lookingThroughBinary);
        void cleanAndAttachClauses(vec<XorClause*>& cs);
        const bool cleanClause(Clause*& ps);
        const bool cleanClause(XorClause& ps);

        Solver& solver;
};