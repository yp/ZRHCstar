/**
 *
 *                              ZRHC-*
 * Zero-Recombinant Haplotype Configuration with missing genotypes
 *
 * Copyright (C) 2010  Yuri Pirola <yuri.pirola(-at-)gmail.com>
 *
 * Distributed under the terms of the GNU General Public License (GPL)
 *
 *
 * This file is part of ZRHC-* (ZRHCstar).
 *
 * ZRHC-* is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZRHC-* is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZRHC-*.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
/**
 *
 * sat_solver_interface.hpp
 *
 * Structures to represent a light-weight interface to an internal SAT solver.
 * It only wants to narrow down the interface to the SAT solver.
 *
 **/

#ifndef __SAT_SOLVER_INTERFACE_HPP__
#define __SAT_SOLVER_INTERFACE_HPP__

// Check that the 'right' preprocessor symbols have been defined:
// NO_INTERNAL_SAT_SOLVER
// INTERNAL_SAT_SOLVER
// ONLY_INTERNAL_SAT_SOLVER (implies INTERNAL_SAT_SOLVER)
//
// 1- check at least one is defined
#if !defined(ONLY_INTERNAL_SAT_SOLVER) && !defined(INTERNAL_SAT_SOLVER) && !defined(NO_INTERNAL_SAT_SOLVER)
// default= ONLY_INTERNAL_SAT_SOLVER
#define ONLY_INTERNAL_SAT_SOLVER
#endif

// 2- ONLY_INTERNAL_SAT_SOLVER => INTERNAL_SAT_SOLVER
#if defined(ONLY_INTERNAL_SAT_SOLVER) && !defined(INTERNAL_SAT_SOLVER)
#define INTERNAL_SAT_SOLVER
#endif

// 3- NO_INTERNAL_SAT_SOLVER => not INTERNAL_SAT_SOLVER
#if defined(NO_INTERNAL_SAT_SOLVER) && defined(INTERNAL_SAT_SOLVER)
#error "We have to choose only one alternative: NO_INTERNAL_SAT_SOLVER or INTERNAL_SAT_SOLVER"
#endif


#ifndef INTERNAL_SAT_SOLVER
#ifdef SAT_SOLVER
#undef SAT_SOLVER
#endif
#define SAT_SOLVER NO internal SAT solver
#endif

// Provide the interface only if asked to do so
#ifdef INTERNAL_SAT_SOLVER

#include "log.hpp"
#include "utility.hpp"

#include <set>

#include "Solver.h"


class SAT_solver_iface_t
  :
  public log_able_t<SAT_solver_iface_t>
{
private:
  Solver* _solver;

  bool _solved;
  bool _sat;

public:
  SAT_solver_iface_t()
		:_solver(new Solver()), _solved(false), _sat(false)
  {};

  ~SAT_solver_iface_t() {
	 delete _solver;
  };

  bool is_solved() const {
	 return _solved;
  };

  void add_clause(const std::set<int>& clause);

  void add_xor_clause(const std::set<int>& clause);

  bool solve();

  bool model(const unsigned int var) const;

  size_t no_of_vars() const {
	 return _solver->nVars();
  };

};

#endif

#endif // __SAT_SOLVER_INTERFACE_HPP__
