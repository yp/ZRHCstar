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
 * anf_constraints.hpp
 *
 * Functions that convert a constraint to a set of clauses
 *
 **/

#ifndef __ANF_CONSTRAINTS_HPP__
#define __ANF_CONSTRAINTS_HPP__

#include "data.hpp"
#include "pedcnf.hpp"
#include "pedcnf_ext.hpp"
#include "expr_tree.hpp"

#include <boost/static_assert.hpp>

template <
  typename PEDCNF_T
>
void
add_anf_constraint(const expr_tree_node& constraint,
						 PEDCNF_T& cnf);


template <>
void
add_anf_constraint<pedcnf_t>(const expr_tree_node& constraint,
									  pedcnf_t& cnf);

template <>
void
add_anf_constraint<pedcnf_ext_t>(const expr_tree_node& constraint,
											pedcnf_ext_t& cnf);

#endif // __ANF_CONSTRAINTS_HPP__
