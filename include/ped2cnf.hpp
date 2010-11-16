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
 * ped2cnf.hpp
 *
 * Functions to convert a pedigree into a SAT instance
 *
 **/

#ifndef __PED2CNF_HPP__
#define __PED2CNF_HPP__


#include "data.hpp"
#include "log.hpp"
#include "tree.hpp"
#include "utility.hpp"
#include "pedcnf.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>


void
add_anf_constraint(const tree_node& constraint,
						 pedcnf_t& cnf);



template <>
inline const char* logger_name<pedcnf_t>(void) {
  return "pedcnf_t";
}

template <
  typename T_GENOTYPE,
  typename T_HAPLOTYPE,
  typename T_PHENOTYPE,
  typename T_ID
  >
class ped2cnf_conv_t:
  public log_able_t< pedcnf_t >,
  public std::unary_function< basic_pedigree_t<T_GENOTYPE,
															  T_HAPLOTYPE,
															  T_PHENOTYPE,
															  T_ID>,
										void >
{
// Types
private:

  typedef basic_pedigree_t<T_GENOTYPE,
									T_HAPLOTYPE,
									T_PHENOTYPE,
									T_ID> pedigree_t;
  typedef typename pedigree_t::individual_t individual_t;
  typedef typename pedigree_t::g g;



public:

// Data members
private:

public:

// Methods
private:

  int homozygous_genotype_to_code(const g& gen) const {
	 MY_ASSERT(is_homozigous(gen));
	 if (gen == g::HOMO1) {
		return 0;
	 } else {
		MY_ASSERT(gen == g::HOMO2);
		return 1;
	 }
  }

  void add_constraint(pedcnf_t& cnf,
							 const g& parent_g,
							 const g& individual_g,
							 const size_t locus,
							 const size_t progr_id_parent,
							 const size_t progr_id_ind,
							 const bool is_mother) {
	 tree_node constraint("xor");
	 int constant= 0;
// First side of the equation
	 if (is_homozigous(parent_g)) {
		constant+= homozygous_genotype_to_code(parent_g);
	 } else {
		tree_node hpl(cnf.get_h(progr_id_parent, locus));
// + hpl
		constraint.children.push_back(hpl);
		tree_node spi(cnf.get_s(progr_id_parent, progr_id_ind));
		if (is_heterozygous(parent_g)) {
// + spi
		  constraint.children.push_back(spi);
		} else {
		  MY_ASSERT(!is_genotyped(parent_g));
// + (spi * wpl)
		  tree_node wpl(cnf.get_w(progr_id_parent, locus));
		  tree_node int_constraint("and");
		  int_constraint.children.push_back(spi);
		  int_constraint.children.push_back(wpl);
		  constraint.children.push_back(int_constraint);
		}
	 }
// Second side of the equation
	 if (is_homozigous(individual_g)) {
		constant+= homozygous_genotype_to_code(individual_g);
	 } else {
		tree_node hil(cnf.get_h(progr_id_ind, locus));
// + hil
		constraint.children.push_back(hil);
		if (is_heterozygous(individual_g)) {
		  if (is_mother) {
			 constant+= 1;
		  } else {
// do nothing
		  }
		} else {
		  MY_ASSERT(!is_genotyped(individual_g));
		  if (is_mother) {
			 tree_node wil(cnf.get_w(progr_id_ind, locus));
// + wil
			 constraint.children.push_back(wil);
		  }
		}
	 }
	 constant = constant % 2;
	 if (constraint.children.empty()) {
		MY_ASSERT(constant == 0);
		L_TRACE("     ANF: empty");
	 } else {
		if (constraint.children.size() == 1) {
		  constraint= constraint.children.front();
		}
		if (constant == 0) {
		  tree_node new_constraint("not");
		  new_constraint.children.push_back(constraint);
		  constraint= new_constraint;
		}
		L_TRACE("     ANF: " << constraint);

		add_anf_constraint(constraint, cnf);
	 }
  };

public:

  pedcnf_t convert(const pedigree_t& ped) {
	 pedcnf_t cnf;
	 BOOST_FOREACH( const individual_t& ind,
						 ped.individuals() ) {
		L_TRACE("Considering individual " << ind.progr_id());
		if (ind.has_father()) {
		  L_TRACE(" --> father " << ind.father().progr_id());
		  const individual_t& parent= ind.father();
		  for (size_t l= 0; l < ped.genotype_length(); ++l) {
			 L_TRACE("  locus = " << l <<
						", g_f = " << parent.g(l) <<
						", g_i = " << ind.g(l));
			 add_constraint(cnf,
								 parent.g(l), ind.g(l),
								 l,
								 parent.progr_id(), ind.progr_id(),
								 false);
		  }
		}
		if (ind.has_mother()) {
		  L_TRACE(" --> mother " << ind.mother().progr_id());
		  const individual_t& parent= ind.mother();
		  for (size_t l= 0; l < ped.genotype_length(); ++l) {
			 L_TRACE("  locus = " << l <<
						", g_f = " << parent.g(l) <<
						", g_i = " << ind.g(l));
			 add_constraint(cnf,
								 parent.g(l), ind.g(l),
								 l,
								 parent.progr_id(), ind.progr_id(),
								 true);
		  }
		}
	 }
	 if (logger()->isDebugEnabled()) {
		L_DEBUG("SAT variables (" << std::setw(4) << cnf.vars().size() << ")");
		if (logger()->isTraceEnabled()) {
		  size_t i= 1;
		  for (pedcnf_t::varvec_t::const_iterator it= cnf.vars().begin();
				 it != cnf.vars().end();
				 ++it, ++i) {
			 L_TRACE(std::setw(4) << i << ")  " << *it);
		  }
		}
		L_DEBUG("SAT clauses   (" << std::setw(4) << cnf.clauses().size() << ")");
		if (logger()->isTraceEnabled()) {
		  size_t i= 0;
		  for (pedcnf_t::clauses_t::const_iterator it= cnf.clauses().begin();
				 it != cnf.clauses().end();
				 ++it, ++i) {
			 L_TRACE(std::setw(4) << i << ")  " << tostr(*it));
		  }
		}
	 }
	 return cnf;
  };
};

template <
  typename T_GENOTYPE,
  typename T_HAPLOTYPE,
  typename T_PHENOTYPE,
  typename T_ID
>
pedcnf_t
ped2cnf(const basic_pedigree_t<T_GENOTYPE,
						T_HAPLOTYPE,
						T_PHENOTYPE,
						T_ID>& ped) {
  ped2cnf_conv_t<T_GENOTYPE, T_HAPLOTYPE, T_PHENOTYPE, T_ID> conv;
  pedcnf_t cnf= conv.convert(ped);
  return cnf;
}


#endif // __PED2CNF_HPP__
