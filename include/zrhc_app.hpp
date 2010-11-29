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
#include "configuration.h"

#include "application.hpp"
#include "log.hpp"
#include "assertion.hpp"
#include "data.hpp"
#include "utility.hpp"

#include "io-pedigree.hpp"
#include "ped2cnf.hpp"
#include "pedcnf2hc.hpp"

#include <iostream>

using namespace std;


class zrhcstar_t:
  public log_able_t<zrhcstar_t>
{

private:

  typedef plink_reader_t<>::multifamily_pedigree_t pedigree_t;
  typedef pedigree_t::pedigree_t family_t;

  void prepare_pedigree_and_sat(std::istream& ped_is,
										  pedigree_t& mped,
										  pedcnf_t& cnf) const {
	 L_INFO("Reading pedigree...");
	 biallelic_genotype_reader_t<> gr;
	 plink_reader_t<> reader(gr);
	 reader.read(ped_is, mped);
	 L_INFO("Pedigree successfully read.");

	 if (mped.families().empty()) {
		throw std::logic_error(std::string("No family has been read."));
	 }
	 if (mped.families().size() > 1) {
		throw std::logic_error(std::string("The pedigree has more than one family."));
	 }

// Prepare the SAT instance
	 L_INFO("Preparing SAT instance from pedigree...");
	 cnf= ped2cnf(mped.families().front());
	 L_INFO("SAT instance successfully prepared.");
  }



  void save_ZRHC(pedigree_t& ped,
					  std::ostream& hap_os) const {
	 L_INFO("Saving haplotype configuration...");
// FIXME: Improve template instantiation
	 biallelic_haplotype_pair_writer_t<> hpw;
	 plink_haplotype_writer_t<> writer(hpw, "\t", "|");
	 writer.write(hap_os, ped);
	 L_INFO("Haplotype configuration successfully saved.");
  };

  bool read_SAT_results(pedcnf_t& cnf,
								std::istream& res_is) const {
	 L_INFO("Reading SAT results...");
	 const bool is_sat= cnf.assignment_from_minisat_format(res_is);
	 L_INFO("SAT results successfully read.");
	 return is_sat;
  };


public:

  void create_SAT_instance_from_pedigree(std::istream& ped_is,
													  std::ostream& sat_os,
													  const std::vector<std::string>& headers) const {
	 pedigree_t ped;
	 pedcnf_t cnf;
	 prepare_pedigree_and_sat(ped_is, ped, cnf);
// Output the instance
	 L_INFO("Saving SAT instance...");
	 cnf.clauses_to_dimacs_format(sat_os, headers);
	 L_INFO("SAT instance successfully saved.");
  };



  bool compute_HC_from_SAT_results(std::istream& ped_is,
											  std::istream& res_is,
											  std::ostream& hap_os) const {
	 pedigree_t ped;
	 pedcnf_t cnf;
	 prepare_pedigree_and_sat(ped_is, ped, cnf);

// Open the result file and read the assignment
	 const bool is_sat= read_SAT_results(cnf, res_is);

	 if (is_sat) {
		L_INFO("The pedigree can be realized by a zero-recombinant haplotype "
			  "configuration.");
		pedigree_t::pedigree_t& family= ped.families().front();
// Compute the actual haplotype configuration
		compute_ZRHC_from_SAT(family, cnf);
// Check the haplotype configuration
		const bool ok=
		  family.is_completely_haplotyped() &&
		  family.is_consistent() &&
		  family.is_zero_recombinant();
		if (ok) {
		  L_INFO("The computed haplotype configuration is valid.");
		} else {
		  L_ERROR("The computed haplotype configuration is not valid.");
		  MY_FAIL;
		}
// Output the haplotype configuration
		save_ZRHC(ped, hap_os);
	 } else {
		L_INFO("The pedigree CANNOT be realized by a zero-recombinant haplotype "
			  "configuration.");
// Do nothing
	 }
	 return is_sat;
  }


};
