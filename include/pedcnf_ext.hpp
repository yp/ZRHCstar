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
 * pedcnf_ext.hpp
 *
 * Structures to represent SAT instances derived from pedigrees.
 * SAT instances are in a mixed format: they are described by a
 * conjunction of or- or xor-clauses.
 *
 **/

#ifndef __PEDCNF_EXT_HPP__
#define __PEDCNF_EXT_HPP__


#include "pedcnf.hpp"


class pedcnf_ext_t
  :
  public pedcnf_t
{

// Types
private:

public:

  typedef std::set<int> xor_clause_t;

#ifndef ONLY_INTERNAL_SAT_SOLVER
  typedef std::set< xor_clause_t > xor_clauses_t;
#endif // ONLY_INTERNAL_SAT_SOLVER

// Data members
private:

  size_t _no_of_xor_clauses;
#ifndef ONLY_INTERNAL_SAT_SOLVER
  xor_clauses_t _xor_clauses;
#endif // ONLY_INTERNAL_SAT_SOLVER

public:


// Methods
private:


public:

  pedcnf_ext_t()
		:_no_of_xor_clauses(0)
  {};

// Override functions

  virtual ~pedcnf_ext_t() {
  };

  virtual size_t no_of_clauses() const {
	 return pedcnf_t::no_of_clauses() + _no_of_xor_clauses;
  };

#ifndef ONLY_INTERNAL_SAT_SOLVER
  virtual bool is_satisfying_assignment() const;

  virtual std::ostream& clauses_to_dimacs_format(std::ostream& out,
																 const std::vector< std::string >& notes) const;

// New functions

  const xor_clauses_t& xor_clauses() const {
	 return _xor_clauses;
  };

#endif // ONLY_INTERNAL_SAT_SOLVER

  void add_xor_clause(const xor_clause_t& clause);

};



#endif // __PEDCNF_EXT_HPP__
