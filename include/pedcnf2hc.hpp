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
 * pedcnf2hc.hpp
 *
 * Functions to convert a satisfying assignment to a zero-recombinant haplotype
 * configuration.
 *
 **/

#ifndef __PEDCNF2HC_HPP__
#define __PEDCNF2HC_HPP__

#include "pedigree.hpp"
#include "pedcnf.hpp"
#include "log.hpp"

#include <log4cxx/logger.h>

#include <boost/foreach.hpp>


template <typename T_GENOTYPE,
			 typename T_HAPLOTYPE,
			 typename T_PHENOTYPE,
			 typename T_ID>
void compute_ZRHC_from_SAT(basic_pedigree_t<T_GENOTYPE, T_HAPLOTYPE, T_PHENOTYPE, T_ID>& ped,
									const pedcnf_t& cnf) {
  log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("pedcnf2hc"));
  typedef basic_pedigree_t<T_GENOTYPE, T_HAPLOTYPE, T_PHENOTYPE, T_ID> family_t;
  INFO("Computing the zero-recombinant haplotype configuration...");
// For each locus in each individual:
//   (1) the locus is genotyped and it is homozygous (thus the haplotype
//       is 'fixed'), or
//   (2) there exists a H_i[j] variable in the SAT instance.
  BOOST_FOREACH( typename family_t::individual_t& ind,
					  ped.individuals() ) {
	 TRACE("Considering individual " << ind.progr_id());
	 for (size_t locus= 0; locus < ped.genotype_length(); ++locus) {
		if ( ! is_genotyped(ind.g(locus)) ) {
//        Individual not genotyped ->
//          -> imputing genotype based on variables w_i_l and h_i_l
		  TRACE("Individual " << ind.progr_id() << " at locus " << locus
				  << " is not genotyped.");
// FIXME: Some variables could not exist.
		  bool hil= cnf.h(ind.progr_id(), locus);
		  bool wil= cnf.w(ind.progr_id(), locus);
		  TRACE("hil " << hil << "   wil " << wil);
		  if ( ! wil) {
			 DEBUG("Not-genotyped individual " << ind.progr_id() <<
					 " at locus " << locus << " is imputed as homozygous.");
			 if ( ! hil ) {
				ind.g(locus)= family_t::g::HOMO1;
			 } else {
				ind.g(locus)= family_t::g::HOMO2;
			 }
		  } else {
			 DEBUG("Not-genotyped individual " << ind.progr_id() <<
					 " at locus " << locus << " is imputed as heterozygous.");
			 ind.g(locus)= family_t::g::HETER;
		  }
		}
		if ( is_genotyped(ind.g(locus)) ) {
		  if ( is_homozigous(ind.g(locus)) ) {
//          Individual genotyped and homozygous ->
//            -> haplotype is fixed and predetermined
			 TRACE("Individual " << ind.progr_id() << " at locus " << locus
					 << " is genotyped and homozygous.");
			 ind.hp(locus)= ind.hm(locus)=
				homozygous_to_haplotype<typename family_t::h, typename family_t::g>(ind.g(locus));
		  } else {
//          Individual genotyped and heterozygous ->
//            -> haplotype depends on variable h_i_l
			 MY_ASSERT( is_heterozygous(ind.g(locus)) );
			 TRACE("Individual " << ind.progr_id() << " at locus " << locus
					 << " is genotyped and heterozygous.");
			 bool hil= cnf.h(ind.progr_id(), locus);
			 if ( ! hil) {
				ind.hp(locus)= family_t::h::ALLELE1;
				ind.hm(locus)= family_t::h::ALLELE2;
			 } else {
				ind.hp(locus)= family_t::h::ALLELE2;
				ind.hm(locus)= family_t::h::ALLELE1;
			 }
		  }
		} else {
		  MY_FAIL;
		}
	 }
  }
  INFO("Zero-recombinant haplotype configuration successfully computed.");
};



#endif // __PEDCNF2HC_HPP__
