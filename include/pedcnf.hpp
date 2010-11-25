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
 * pedcnf.hpp
 *
 * Structures to represent SAT instances derived from pedigrees.
 *
 **/

#ifndef __PEDCNF_HPP__
#define __PEDCNF_HPP__


#include "data.hpp"
#include "log.hpp"
#include "utility.hpp"

#include <map>
#include <set>
#include <vector>
#include <ostream>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>


class ped_var_kind
  :public enum_like_t<ped_var_kind, 4, 3>
{
private:

  typedef enum_like_t<ped_var_kind, 4, 3> base;

  ped_var_kind(const int val)
		:base(val)
  {};

public:

  ped_var_kind(const ped_var_kind& pvk)
		:base(pvk)
  {};

  static const ped_var_kind H;
  static const ped_var_kind W;
  static const ped_var_kind S;
  static const ped_var_kind DUMMY;

  static const int int_values[];
  static const std::string str_values[];
  static const ped_var_kind enum_values[];

};


class pedcnf_t
  :
  public log_able_t<pedcnf_t>
{

// Types
private:

  typedef boost::tuple<size_t, size_t> index_var_t;

public:

  typedef boost::tuple<ped_var_kind, size_t, size_t> pedvar_t;
  typedef std::map<index_var_t, int> varmap_t;
  typedef std::vector<pedvar_t> varvec_t;
  typedef std::vector<bool> valvec_t;
  typedef std::set<int> clause_t;
  typedef std::set< clause_t > clauses_t;

// Data members
private:

  varmap_t _h;
  varmap_t _w;
  varmap_t _s;
  varmap_t _dummy;

  varvec_t _vars;
  valvec_t _vals;

  clauses_t _clauses;

public:


// Methods
private:

  int get_var(varmap_t& map,
				  const ped_var_kind& var_kind,
				  const size_t i1, const size_t i2);

  int get_var(const varmap_t& map,
				  const size_t i1, const size_t i2) const;

  bool get_val(const varmap_t& map,
					const size_t i1, const size_t i2) const;


public:

  int get_h(const size_t i, const size_t l);

  int get_w(const size_t i, const size_t l);

  int get_s(const size_t p, const size_t i);

  int get_dummy(const int v1, const int v2);

  int get_h(const size_t i, const size_t l) const;

  int get_w(const size_t i, const size_t l) const;

  int get_s(const size_t p, const size_t i) const;

  int get_dummy(const int v1, const int v2) const;

  bool h(const size_t i, const size_t l) const {
	 return get_val(_h, i, l);
  };

  bool w(const size_t i, const size_t l) const {
	 return get_val(_w, i, l);
  };

  bool s(const size_t p, const size_t i) const {
	 return get_val(_s, p, i);
  };

  bool dummy(const int v1, const int v2) const {
	 return get_val(_dummy, v1, v2);
  };

  const varmap_t& h() const {
	 return _h;
  };

  const varmap_t& w() const {
	 return _w;
  };

  const varmap_t& s() const {
	 return _s;
  };

  const varmap_t& dummy() const {
	 return _dummy;
  };

  const varvec_t& vars() const {
	 return _vars;
  };

  const valvec_t& vals() const {
	 return _vals;
  };

  const clauses_t& clauses() const {
	 return _clauses;
  };

  void add_clause(const clause_t& clause) {
	 _clauses.insert(clause);
  };

  bool is_satisfying_assignment() const;

  std::ostream& clauses_to_dimacs_format(std::ostream& out) const;
  std::ostream& clauses_to_dimacs_format(std::ostream& out,
													  const std::string& note) const;
  std::ostream& clauses_to_dimacs_format(std::ostream& out,
													  const std::vector< std::string >& notes) const;

  std::string clauses_to_dimacs_format() const {
	 std::ostringstream out;
	 clauses_to_dimacs_format(out);
	 return out.str();
  };
  std::string clauses_to_dimacs_format(const std::string& note) const {
	 std::ostringstream out;
	 clauses_to_dimacs_format(out, note);
	 return out.str();
  };
  std::string clauses_to_dimacs_format(const std::vector< std::string > notes) const {
	 std::ostringstream out;
	 clauses_to_dimacs_format(out, notes);
	 return out.str();
  };

// Read the assignment from a file like the following one:
// SAT/UNSAT
// 1 -2 3 4 0
  bool assignment_from_minisat_format(std::istream& in);

};



std::ostream&
operator<<(std::ostream& out, const pedcnf_t::pedvar_t& var);

std::ostream&
operator<<(std::ostream& out, const pedcnf_t::clause_t& clause);



#endif // __PEDCNF_HPP__
