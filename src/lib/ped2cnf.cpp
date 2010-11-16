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
 * ped2cnf.cpp
 *
 * Functions to convert a pedigree into a SAT instance
 *
 **/

#include "ped2cnf.hpp"

static void
assign(pedcnf_t& cnf, std::vector<int>& v,
				const size_t n, const size_t index) {
  if (n==0) {
	 pedcnf_t::clause_t clause;
	 std::copy(v.begin(), v.end(), std::inserter(clause, clause.begin()));
	 cnf.add_clause(clause);
  } else {
// n >= 1
	 for (size_t i= index; i <= v.size()-n; ++i) {
		v[i]= -v[i];
		assign(cnf, v, n-1, i+1);
		v[i]= -v[i];
	 }
  }
}

static void
add_constraints_for_dummy_variables(pedcnf_t& cnf,
												int var1,
												int var2,
												const int dummy) {
  if (var1>var2)
	 std::swap(var1, var2);
  pedcnf_t::clause_t clause;
// (var1 or not dummy)
  clause.clear();
  clause.insert(var1);
  clause.insert(-dummy);
  cnf.add_clause(clause);
// (var2 or not dummy)
  clause.clear();
  clause.insert(var2);
  clause.insert(-dummy);
  cnf.add_clause(clause);
// (not var1 or not var2 or dummy)
  clause.clear();
  clause.insert(-var1);
  clause.insert(-var2);
  clause.insert(dummy);
  cnf.add_clause(clause);
}

static void
get_basic_clause(pedcnf_t& cnf,
					  const tree_node& n,
					  std::vector<int>& v) {
  MY_ASSERT( boost::any_cast<std::string>(&n.data) );
  MY_ASSERT( boost::any_cast<std::string>(n.data) == "xor" );
  v.clear();
  v.reserve(n.children.size());
  for (std::list<tree_node>::const_iterator it= n.children.begin();
		 it != n.children.end();
		 ++it) {
	 const tree_node& child= *it;
	 if (boost::any_cast<std::string>(&child.data)) {
		MY_ASSERT( boost::any_cast<std::string>(child.data) == "and" );
		MY_ASSERT( child.children.size() == 2 );
		MY_ASSERT( boost::any_cast<int>(&child.children.front().data) );
		MY_ASSERT( boost::any_cast<int>(&child.children.back().data) );
		int var1= boost::any_cast<int>(child.children.front().data);
		int var2= boost::any_cast<int>(child.children.back().data);
		int dummy= cnf.get_dummy(var1, var2);
		add_constraints_for_dummy_variables(cnf, var1, var2, dummy);
		v.push_back(dummy);
	 } else if (boost::any_cast<int>(&child.data)) {
		int var= boost::any_cast<int>(child.data);
		MY_ASSERT( var>=0 );
		v.push_back(var);
	 }
  }
}

static void
not2cnf(const tree_node& constraint,
		  pedcnf_t& cnf) {
  MY_ASSERT( constraint.children.size()==1 );
  const tree_node& child= constraint.children.front();
  if (boost::any_cast<std::string>(&child.data)) {
	 MY_ASSERT( boost::any_cast<std::string>(child.data) == "xor" );
	 std::vector<int> basic_clause;
	 get_basic_clause(cnf, child, basic_clause);
	 for (size_t n= 1; n<=basic_clause.size(); n+= 2) {
		assign(cnf, basic_clause, n, 0);
	 }
  } else if (boost::any_cast<int>(&child.data)) {
	 int var= boost::any_cast<int>(child.data);
	 MY_ASSERT( var>=0 );
	 pedcnf_t::clause_t clause;
	 clause.insert(-var);
	 cnf.add_clause(clause);
  } else {
	 MY_FAIL;
  }
}

static void
xor2cnf(const tree_node& constraint,
		  pedcnf_t& cnf) {
  MY_ASSERT( constraint.children.size()>1 );
  MY_ASSERT( boost::any_cast<std::string>(&constraint.data) );
  MY_ASSERT( boost::any_cast<std::string>(constraint.data) == "xor" );
  std::vector<int> basic_clause;
  get_basic_clause(cnf, constraint, basic_clause);
  for (size_t n= 0; n<=basic_clause.size(); n+= 2) {
	 assign(cnf, basic_clause, n, 0);
  }
}

void
add_anf_constraint(const tree_node& constraint,
						 pedcnf_t& cnf) {
  if (boost::any_cast<std::string>(&constraint.data)) {
	 const std::string& data= boost::any_cast<std::string>(constraint.data);
	 if (data == "not") {
		not2cnf(constraint, cnf);
	 } else if (data == "xor") {
		xor2cnf(constraint, cnf);
	 } else {
// Should not arrive here
		MY_FAIL;
	 }
  } else if (boost::any_cast<int>(&constraint.data)) {
	 int var= boost::any_cast<int>(constraint.data);
	 MY_ASSERT( var>=0 );
	 pedcnf_t::clause_t clause;
	 clause.insert(var);
	 cnf.add_clause(clause);
  }
};

