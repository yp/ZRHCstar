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
 * pedcnf_ext.cpp
 *
 * Structures to represent SAT instances derived from pedigrees.
 * SAT instances are in a mixed format: they are described by a
 * conjunction of or- or xor-clauses.
 *
 **/

#include "pedcnf_ext.hpp"
#include <iomanip>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>

#ifndef ONLY_INTERNAL_SAT_SOLVER
bool
pedcnf_ext_t::is_satisfying_assignment() const {
  L_DEBUG("Checking if value assignment satisfies the or- and xor-clauses...");
  const bool base_ris= this->pedcnf_t::is_satisfying_assignment();
  if (!base_ris) {
	 L_DEBUG("The value assignment does not satisfy the or-clauses.");
	 return false;
  }
  MY_ASSERT_DBG( vars().size() == vals().size() );
  bool ris= true;
  BOOST_FOREACH(const xor_clause_t& clause, _xor_clauses) {
	 if (ris) {
		bool intris= false;
		BOOST_FOREACH(const int& var, clause) {
		  MY_ASSERT( var != 0 );
		  MY_ASSERT( (size_t)abs(var) <= vars().size() );
		  bool val_var= vals()[abs(var)-1];
		  if (var<0) val_var= !val_var;
		  intris= (intris && !val_var) || (!intris && val_var);
		}
		if (!intris) {
		  L_DEBUG("Clause " << tostr(clause) << " is not satisfied.");
		}
		ris= ris && intris;
	 }
  }
  if (ris) {
	 L_DEBUG("The assignment satisfies all the or- and xor-clauses.");
  } else {
	 L_DEBUG("The assignment does not satisfy the xor-clauses.");
  }
  return ris;
};


std::ostream&
pedcnf_ext_t::clauses_to_dimacs_format(std::ostream& out,
													const std::vector< std::string >& notes) const {
  BOOST_FOREACH(const std::string& s, notes) {
	 out << "c " << s << std::endl;
  }
  out << "c extended syntax: or- and xor-clauses" << std::endl;
  out << "c" << std::endl;
  size_t i= 1;
  for (pedcnf_t::varvec_t::const_iterator it= vars().begin();
		 it != vars().end();
		 ++it, ++i) {
	 out << "c v " << std::setw(5) << i << " " << *it << std::endl;
  }
  out << "c" << std::endl;
  out << "p cnf " << vars().size() << " " <<
	 (clauses().size()+_xor_clauses.size()) << std::endl;
  for (clauses_t::const_iterator it= clauses().begin();
		 it != clauses().end();
		 ++it) {
	 out << *it << std::endl;
  }
  for (xor_clauses_t::const_iterator it= _xor_clauses.begin();
		 it != _xor_clauses.end();
		 ++it) {
	 out << "x" << *it << std::endl;
  }
  return out;
};

#endif // ONLY_INTERNAL_SAT_SOLVER

void
pedcnf_ext_t::add_xor_clause(const xor_clause_t& clause) {
#ifndef ONLY_INTERNAL_SAT_SOLVER
  _xor_clauses.insert(clause);
#endif
#ifdef INTERNAL_SAT_SOLVER
  _solver.add_xor_clause(clause);
#endif
  ++_no_of_xor_clauses;
};
