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

#include "tree.hpp"

#include "log.hpp"
#include "assertion.hpp"


std::ostream&
print_boost_any(std::ostream& out, const boost::any& i) {
  if (boost::any_cast<int>(&i)) out << boost::any_cast<int>(i);
  else if (boost::any_cast<std::string>(&i)) out << boost::any_cast<std::string>(i);
  else out << " IMPOSSIBLE TO WRITE boost::any VALUE ";
  return out;
}

std::ostream&
operator<< (std::ostream& out, const tree_node& node) {
  MY_ASSERT( ( !node.data.empty() ) || node.children.empty() );
  if (!node.data.empty()) {
	 if (node.children.empty()) {
		print_boost_any(out, node.data);
	 } else {
		out << "( ";
		bool first= node.children.size()>1;
		for (std::list<tree_node>::const_iterator it= node.children.begin();
			  it != node.children.end();
			  ++it) {
		  if (!first) {
			 print_boost_any(out, node.data) << " ";
		  }
		  out << (*it) << " ";
		  first= false;
		}
		out << ")";
	 }
  }
  return out;
};

