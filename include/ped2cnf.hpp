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
#include "expr_tree.hpp"
#include "utility.hpp"
#include "pedcnf.hpp"
#include "anf_constraints.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>




template <>
inline const char* logger_name<pedcnf_t>(void) {
  return "pedcnf_t";
}

template <
  typename T_GENOTYPE,
  typename T_HAPLOTYPE,
  typename T_PHENOTYPE,
  typename T_ID,
  typename T_PEDCNF
  >
class ped2cnf_conv_t:
  public log_able_t< T_PEDCNF >,
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
	 MY_ASSERT_DBG(is_homozigous(gen));
	 if (gen == g::HOMO1) {
		return 0;
	 } else {
		MY_ASSERT(gen == g::HOMO2);
		return 1;
	 }
  }

  void add_constraint(T_PEDCNF& cnf,
							 const g& parent_g,
							 const g& individual_g,
							 const size_t locus,
							 const size_t progr_id_parent,
							 const size_t progr_id_ind,
							 const bool is_mother) {
	 expr_operator_t constraint(EXPR_OP_XOR);
	 int constant= 0;
// First side of the equation
	 if (is_homozigous(parent_g)) {
		constant+= homozygous_genotype_to_code(parent_g);
	 } else {
		expr_variable_t* hpl= new expr_variable_t(cnf.get_h(progr_id_parent, locus));
// + hpl
		constraint.children.push_back(hpl);
		expr_variable_t* spi= new expr_variable_t(cnf.get_s(progr_id_parent, progr_id_ind));
		if (is_heterozygous(parent_g)) {
// + spi
		  constraint.children.push_back(spi);
		} else {
		  MY_ASSERT_DBG(!is_genotyped(parent_g));
// + (spi * wpl)
		  expr_variable_t* wpl= new expr_variable_t(cnf.get_w(progr_id_parent, locus));
		  expr_operator_t* int_constraint= new expr_operator_t(EXPR_OP_AND);
		  int_constraint->children.push_back(spi);
		  int_constraint->children.push_back(wpl);
		  constraint.children.push_back(int_constraint);
		}
	 }
// Second side of the equation
	 if (is_homozigous(individual_g)) {
		constant+= homozygous_genotype_to_code(individual_g);
	 } else {
		expr_variable_t* hil= new expr_variable_t(cnf.get_h(progr_id_ind, locus));
// + hil
		constraint.children.push_back(hil);
		if (is_heterozygous(individual_g)) {
		  if (is_mother) {
			 constant+= 1;
		  } else {
// do nothing
		  }
		} else {
		  MY_ASSERT_DBG(!is_genotyped(individual_g));
		  if (is_mother) {
			 expr_variable_t* wil= new expr_variable_t(cnf.get_w(progr_id_ind, locus));
// + wil
			 constraint.children.push_back(wil);
		  }
		}
	 }
	 constant = constant % 2;
	 if (constraint.children.empty()) {
		MY_ASSERT_DBG(constant == 0);
		L_TRACE("     ANF: empty");
	 } else {
		if (constraint.children.size() == 1) {
		  if (constant == 0) {
			 constraint.kind= EXPR_OP_NOT;
			 add_anf_constraint<T_PEDCNF>(constraint, cnf);
		  } else {
			 add_anf_constraint<T_PEDCNF>(constraint.children.front(), cnf);
		  }
		} else {
		  if (constant == 0) {
			 expr_operator_t new_constraint(EXPR_OP_NOT);
			 new_constraint.children.push_back(new expr_operator_t(constraint));
			 add_anf_constraint<T_PEDCNF>(new_constraint, cnf);
		  } else {
			 add_anf_constraint<T_PEDCNF>(constraint, cnf);
		  }
		}
	 }
  };

public:

  T_PEDCNF* convert(const pedigree_t& ped) {
	 T_PEDCNF* pcnf= new T_PEDCNF;
	 T_PEDCNF& cnf= *pcnf;
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
	 L_INFO("The SAT instance is composed by " <<
			  std::setw(8) << cnf.vars().size() << " variables and " <<
			  std::setw(8) << cnf.no_of_clauses() << " clauses");
	 return pcnf;
  };
};

template <
  typename T_GENOTYPE,
  typename T_HAPLOTYPE,
  typename T_PHENOTYPE,
  typename T_ID
>
pedcnf_t*
ped2cnf(const basic_pedigree_t<T_GENOTYPE,
						T_HAPLOTYPE,
						T_PHENOTYPE,
						T_ID>& ped) {
  ped2cnf_conv_t<T_GENOTYPE, T_HAPLOTYPE, T_PHENOTYPE, T_ID, pedcnf_t> conv;
  return conv.convert(ped);
}

template <
  typename T_GENOTYPE,
  typename T_HAPLOTYPE,
  typename T_PHENOTYPE,
  typename T_ID
>
pedcnf_ext_t*
ped2cnf_ext(const basic_pedigree_t<T_GENOTYPE,
											  T_HAPLOTYPE,
											  T_PHENOTYPE,
											  T_ID>& ped) {
  ped2cnf_conv_t<T_GENOTYPE, T_HAPLOTYPE, T_PHENOTYPE, T_ID, pedcnf_ext_t> conv;
  return conv.convert(ped);
}


#endif // __PED2CNF_HPP__
