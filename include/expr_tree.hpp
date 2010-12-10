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
 * expr_tree.hpp
 *
 * Representation of a generic expression tree
 *
 **/

#ifndef __EXPR_TREE_HPP__
#define __EXPR_TREE_HPP__

#include "utility.hpp"

#include <boost/ptr_container/ptr_list.hpp>

#include <ostream>

class expr_tree_node {
protected:

  expr_tree_node()
  {};

private:

  virtual
  expr_tree_node* clone_impl() const = 0;

public:
  virtual ~expr_tree_node()
  {};

  expr_tree_node* clone() const {
	 return clone_impl();
  };

};

expr_tree_node* new_clone(const expr_tree_node& n);

typedef enum {
  EXPR_OP_XOR= 0,
  EXPR_OP_NOT= 1,
  EXPR_OP_AND= 2,
} expr_operator_enum;


class expr_operator_t:
  public expr_tree_node {

public:

  typedef boost::ptr_list<expr_tree_node> children_t;

  expr_operator_enum kind;

  children_t children;

  expr_operator_t(const expr_operator_enum _kind)
		:kind(_kind)
  {};

private:

  expr_operator_t(const expr_operator_enum _kind,
						const children_t& _children)
		:kind(_kind), children(_children)
  {};

  virtual expr_operator_t* clone_impl() const {
	 return new expr_operator_t(kind, children);
  };

};


class expr_variable_t:
  public expr_tree_node {
public:
  int variable;

  expr_variable_t(const int _variable)
		:variable(_variable)
  {};

private:
  virtual expr_variable_t* clone_impl() const {
	 return new expr_variable_t(variable);
  };

};


std::ostream&
operator<< (std::ostream& out, const expr_tree_node& node);

std::ostream&
operator<< (std::ostream& out, const expr_operator_t& op);

std::ostream&
operator<< (std::ostream& out, const expr_variable_t& var);





#endif // __EXPR_TREE_HPP__
