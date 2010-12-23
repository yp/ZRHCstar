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
 * anf_constraints.cpp
 *
 * Functions that convert a constraint to a set of clauses
 *
 **/

#include "anf_constraints.hpp"

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
					  const expr_operator_t& n,
					  std::vector<int>& v) {
  MY_ASSERT_DBG( n.kind == EXPR_OP_XOR );
  v.clear();
  v.reserve(n.children.size());
  for (expr_operator_t::children_t::const_iterator it= n.children.begin();
		 it != n.children.end();
		 ++it) {
	 const expr_tree_node& _it= *it;
	 if (typeid(_it) == typeid(expr_operator_t)) {
		const expr_operator_t& child= dynamic_cast<const expr_operator_t&>(_it);
		MY_ASSERT_DBG( child.kind == EXPR_OP_AND );
		MY_ASSERT_DBG( child.children.size() == 2 );
		MY_ASSERT_DBG( typeid(child.children.front()) == typeid(expr_variable_t) );
		MY_ASSERT_DBG( typeid(child.children.back()) == typeid(expr_variable_t) );
		int var1= dynamic_cast<const expr_variable_t&>(child.children.front()).variable;
		int var2= dynamic_cast<const expr_variable_t&>(child.children.back()).variable;
		int dummy= cnf.get_dummy(var1, var2);
		add_constraints_for_dummy_variables(cnf, var1, var2, dummy);
		v.push_back(dummy);
	 } else if (typeid(*it) == typeid(expr_variable_t)) {
		int var= dynamic_cast<const expr_variable_t&>(*it).variable;
		MY_ASSERT_DBG( var>=0 );
		v.push_back(var);
	 }
  }
}

static void
not2cnf(const expr_operator_t& constraint,
		  pedcnf_t& cnf) {
  MY_ASSERT_DBG( constraint.children.size()==1 );
  const expr_tree_node& _child= constraint.children.front();
  if (typeid(_child) == typeid(expr_operator_t)) {
	 const expr_operator_t& child= dynamic_cast<const expr_operator_t&>(_child);
	 MY_ASSERT_DBG( child.kind == EXPR_OP_XOR );
	 std::vector<int> basic_clause;
	 get_basic_clause(cnf, child, basic_clause);
	 for (size_t n= 1; n<=basic_clause.size(); n+= 2) {
		assign(cnf, basic_clause, n, 0);
	 }
  } else if (typeid(_child) == typeid(expr_variable_t)) {
	 int var= dynamic_cast<const expr_variable_t&>(_child).variable;
	 MY_ASSERT_DBG( var>=0 );
	 pedcnf_t::clause_t clause;
	 clause.insert(-var);
	 cnf.add_clause(clause);
  } else {
	 MY_FAIL;
  }
}

static void
xor2cnf(const expr_operator_t& constraint,
		  pedcnf_t& cnf) {
  MY_ASSERT_DBG( constraint.children.size()>1 );
  MY_ASSERT_DBG( constraint.kind == EXPR_OP_XOR );
  std::vector<int> basic_clause;
  get_basic_clause(cnf, constraint, basic_clause);
  for (size_t n= 0; n<=basic_clause.size(); n+= 2) {
	 assign(cnf, basic_clause, n, 0);
  }
}

static void
_add_anf_constraint(const expr_operator_t& constraint,
						 pedcnf_t& cnf) {
  if (constraint.kind == EXPR_OP_NOT) {
	 not2cnf(constraint, cnf);
  } else if (constraint.kind == EXPR_OP_XOR) {
	 xor2cnf(constraint, cnf);
  } else {
// Should not arrive here
	 MY_FAIL;
  }
};

static void
_add_anf_constraint(const expr_variable_t& constraint,
						 pedcnf_t& cnf) {
  const int var= constraint.variable;
  MY_ASSERT_DBG( var>=0 );
  pedcnf_t::clause_t clause;
  clause.insert(var);
  cnf.add_clause(clause);
};

template <>
void
add_anf_constraint<pedcnf_t>(const expr_tree_node& constraint,
									  pedcnf_t& cnf) {
  if (typeid(constraint) == typeid(expr_operator_t)) {
	 _add_anf_constraint(dynamic_cast<const expr_operator_t&>(constraint), cnf);
  } else if (typeid(constraint) == typeid(expr_variable_t)) {
	 _add_anf_constraint(dynamic_cast<const expr_variable_t&>(constraint), cnf);
  } else {
	 MY_FAIL;
  }
};


static void
not2cnf_ext(const expr_operator_t& constraint,
				pedcnf_ext_t& cnf) {
  MY_ASSERT_DBG( constraint.children.size()==1 );
  const expr_tree_node& _child= constraint.children.front();
  if (typeid(_child) == typeid(expr_operator_t)) {
	 const expr_operator_t& child= dynamic_cast<const expr_operator_t&>(_child);
	 MY_ASSERT_DBG( child.kind == EXPR_OP_XOR );
	 std::vector<int> basic_clause;
	 get_basic_clause(cnf, child, basic_clause);
// There was a NOT in the root: change the sign of the first literal
	 basic_clause[0]= -basic_clause[0];
	 pedcnf_ext_t::xor_clause_t clause;
	 std::copy(basic_clause.begin(), basic_clause.end(),
				  std::inserter(clause, clause.begin()));
	 cnf.add_xor_clause(clause);
  } else if (typeid(_child) == typeid(expr_variable_t)) {
	 int var= dynamic_cast<const expr_variable_t&>(_child).variable;
	 MY_ASSERT_DBG( var>=0 );
	 pedcnf_t::clause_t clause;
	 clause.insert(-var);
	 cnf.add_clause(clause);
  } else {
	 MY_FAIL;
  }
}

static void
xor2cnf_ext(const expr_operator_t& constraint,
				pedcnf_ext_t& cnf) {
  MY_ASSERT_DBG( constraint.children.size()>1 );
  MY_ASSERT_DBG( constraint.kind == EXPR_OP_XOR );
  std::vector<int> basic_clause;
  get_basic_clause(cnf, constraint, basic_clause);
  pedcnf_ext_t::xor_clause_t clause;
  std::copy(basic_clause.begin(), basic_clause.end(),
				std::inserter(clause, clause.begin()));
  cnf.add_xor_clause(clause);
}

static void
_add_anf_constraint_ext(const expr_operator_t& constraint,
								pedcnf_ext_t& cnf) {
  if (constraint.kind == EXPR_OP_NOT) {
	 not2cnf_ext(constraint, cnf);
  } else if (constraint.kind == EXPR_OP_XOR) {
	 xor2cnf_ext(constraint, cnf);
  } else {
// Should not arrive here
	 MY_FAIL;
  }
};

template <>
void
add_anf_constraint<pedcnf_ext_t>(const expr_tree_node& constraint,
											pedcnf_ext_t& cnf) {
  if (typeid(constraint) == typeid(expr_operator_t)) {
	 _add_anf_constraint_ext(dynamic_cast<const expr_operator_t&>(constraint), cnf);
  } else if (typeid(constraint) == typeid(expr_variable_t)) {
	 _add_anf_constraint(dynamic_cast<const expr_variable_t&>(constraint), cnf);
  } else {
	 MY_FAIL;
  }
};
