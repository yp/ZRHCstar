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
 * tree.cpp
 *
 * Representation of a generic tree
 *
 **/

#include "expr_tree.hpp"

#include "log.hpp"
#include "assertion.hpp"

expr_tree_node* new_clone(const expr_tree_node& n) {
  return n.clone();
};

std::ostream&
operator<< (std::ostream& out, const expr_tree_node& node) {
  if (typeid(node) == typeid(expr_operator_t)) {
	 return out << dynamic_cast<const expr_operator_t&>(node);
  } else if (typeid(node) == typeid(expr_variable_t)) {
	 return out << dynamic_cast<const expr_variable_t&>(node);
  } else {
	 MY_FAIL;
  }
  return out;
};


std::ostream&
operator<< (std::ostream& out, const expr_operator_t& op) {
  out << "( ";
  bool first= op.children.size()>1;
  for (expr_operator_t::children_t::const_iterator it= op.children.begin();
		 it != op.children.end();
		 ++it) {
	 out << (*it) << " ";
	 first= false;
  }
  out << ")";
  return out;
};

std::ostream&
operator<< (std::ostream& out, const expr_variable_t& var) {
  out << var.variable;
  return out;
};
