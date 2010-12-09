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

#include <list>

#include <boost/any.hpp>

#include <ostream>

class expr_tree_node {
public:
  boost::any data;
  std::list<expr_tree_node> children;

  expr_tree_node()
  {};

  explicit expr_tree_node(const char* _data)
		:data(std::string(_data))
  {};

  template <typename T>
  explicit expr_tree_node(const T& _data)
		:data(_data)
  {};
};


std::ostream&
operator<< (std::ostream& out, const expr_tree_node& node);





#endif // __EXPR_TREE_HPP__
