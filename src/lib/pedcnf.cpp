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
 * pedcnf.cpp
 *
 * Structures to represent SAT instances derived from pedigrees.
 *
 **/

#include "pedcnf.hpp"
#include <iomanip>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>

const ped_var_kind ped_var_kind::H(0);
const ped_var_kind ped_var_kind::W(1);
const ped_var_kind ped_var_kind::S(2);
const ped_var_kind ped_var_kind::DUMMY(3);
const int ped_var_kind::int_values[]={0, 1, 2, 3};
const std::string ped_var_kind::str_values[]={"h", "w", "s", "dummy"};
const ped_var_kind ped_var_kind::enum_values[]={H, W, S, DUMMY};

std::ostream&
operator<<(std::ostream& out, const pedcnf_t::pedvar_t& var) {
  out << var.get<0>() << "_" << var.get<1>() << "_" << var.get<2>();
  return out;
}


inline int
pedcnf_t::get_var(varmap_t& map,
						const ped_var_kind& var_kind,
						const size_t i1, const size_t i2) {
  varmap_t::iterator it= map.find(boost::make_tuple(i1, i2));
  if (it == map.end()) {
	 _vars.push_back(boost::make_tuple(var_kind, i1, i2));
	 _vals.push_back(false);
	 std::pair< varmap_t::iterator, bool> ret= map.insert(std::make_pair(boost::make_tuple(i1, i2), _vars.size()));
	 MY_ASSERT(ret.second);
	 it= ret.first;
  }
  return it->second;
};

int
pedcnf_t::get_h(const size_t i, const size_t l) {
  return get_var(_h, ped_var_kind::H, i, l);
};

int
pedcnf_t::get_w(const size_t i, const size_t l) {
  return get_var(_w, ped_var_kind::W, i, l);
};

int
pedcnf_t::get_s(const size_t p, const size_t i) {
  return get_var(_s, ped_var_kind::S, p, i);
};

int
pedcnf_t::get_dummy(const int v1, const int v2) {
  return get_var(_dummy, ped_var_kind::DUMMY, v1, v2);
};

bool
pedcnf_t::is_satisfying_assignment() const {
  L_DEBUG("Checking if value assignment satisfies the clauses...");
  MY_ASSERT( _vars.size() == _vals.size() );
  bool ris= true;
  BOOST_FOREACH(const clause_t& clause, _clauses) {
	 if (ris) {
		bool intris= false;
		BOOST_FOREACH(const int& var, clause) {
		  MY_ASSERT( var != 0 );
		  MY_ASSERT( (size_t)abs(var) <= _vars.size() );
		  if (var>0) {
			 intris= intris || _vals[var-1];
		  } else {
			 intris= intris || !_vals[-var-1];
		  }
		}
		if (!intris) {
		  L_DEBUG("Clause " << tostr(clause) << " is not satisfied.");
		}
		ris= ris && intris;
	 }
  }
  if (ris) {
	 L_DEBUG("The assignment satisfies all the clauses.");
  } else {
	 L_DEBUG("The assignment does not satisfy all the clauses.");
  }
  return ris;
};


std::ostream&
pedcnf_t::clauses_to_dimacs_format(std::ostream& out) const {
  return clauses_to_dimacs_format(out, "SAT instance");
}

std::ostream&
pedcnf_t::clauses_to_dimacs_format(std::ostream& out,
											  const std::string& note) const {
  return clauses_to_dimacs_format(out, std::vector< std::string >(1, note));
}

std::ostream&
pedcnf_t::clauses_to_dimacs_format(std::ostream& out,
											  const std::vector< std::string >& notes) const {
  BOOST_FOREACH(const std::string& s, notes) {
	 out << "c " << s << std::endl;
  }
  out << "c" << std::endl;
  size_t i= 1;
  for (pedcnf_t::varvec_t::const_iterator it= _vars.begin();
		 it != _vars.end();
		 ++it, ++i) {
	 out << "c v " << std::setw(5) << i << " " << *it << std::endl;
  }
  out << "c" << std::endl;
  out << "p cnf " << _vars.size() << " " << _clauses.size() << std::endl;
  for (clauses_t::const_iterator it= _clauses.begin();
		 it != _clauses.end();
		 ++it) {
	 out << *it << std::endl;
  }
  return out;
};


// Read the assignment from a file like the following one:
// SAT/UNSAT
// 1 -2 3 4 0
bool
pedcnf_t::assignment_from_minisat_format(std::istream& in) {
  std::string line;
  std::getline(in, line);
  boost::trim(line);
  MY_ASSERT( (line == "SAT") || (line == "UNSAT") );
  if (line == "UNSAT") {
	 L_DEBUG("UNSATISFIABLE clauses.");
	 return false;
  } else if (line == "SAT") {
	 L_DEBUG("Satisfiable clauses. Reading assignment...");
	 std::getline(in, line);
	 boost::trim(line);
	 std::istringstream is(line);
	 int value;
	 while ((is >> value) && (value != 0)) {
		MY_ASSERT( (size_t)abs(value) <= _vals.size() );
		_vals[(size_t)abs(value)-1]= (value>0);
	 }
	 MY_ASSERT( ! (is >> value) ); // zero must be the last element
	 MY_ASSERT( is_satisfying_assignment() );
	 return true;
  } else {
	 MY_FAIL;
  }
  return false;
};


std::ostream&
operator<<(std::ostream& out, const pedcnf_t::clause_t& clause) {
  for (pedcnf_t::clause_t::const_iterator it= clause.begin();
		 it != clause.end();
		 ++it) {
	 out << std::setw(6) << *it << " ";
  }
  out << "     0";
  return out;
};
