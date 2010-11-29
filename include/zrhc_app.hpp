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
#include <fstream>

using namespace std;


class zrhcstar_t:
  public log_able_t<zrhcstar_t>
{

private:

  typedef plink_reader_t<>::multifamily_pedigree_t pedigree_t;
  typedef pedigree_t::pedigree_t family_t;

  void prepare_pedigree_and_sat(const string& ped_file,
										  pedigree_t& mped,
										  pedcnf_t& cnf) const {
// Open the file and read the pedigree
	 ifstream is(ped_file);
	 if (!is.is_open()) {
		L_ERROR("Impossible to open pedigree file '" << ped_file << "'.");
		throw logic_error(string("Impossible to open pedigree file '")
								+ ped_file + "'.");
	 }

	 L_INFO("Reading pedigree from file '" << ped_file << "'...");
	 biallelic_genotype_reader_t<> gr;
	 plink_reader_t<> reader(gr);
	 reader.read(is, mped);
	 is.close();
	 L_INFO("Pedigree successfully read from file '" << ped_file << "'.");

	 if (mped.families().empty()) {
		throw logic_error(string("No family has been read from file '")
								+ ped_file + "'.");
	 }
	 if (mped.families().size() > 1) {
		throw logic_error(string("The pedigree read from file '")
								+ ped_file + "' has more than one family.");
	 }

// Prepare the SAT instance
	 L_INFO("Preparing SAT instance from pedigree...");
	 cnf= ped2cnf(mped.families().front());
	 L_INFO("SAT instance successfully prepared.");
  }



  void save_ZRHC(pedigree_t& ped,
					  const string& hap_file) const {
	 L_INFO("Saving haplotype configuration to file '" << hap_file << "'...");
// FIXME: Improve template instantiation
	 biallelic_haplotype_pair_writer_t<> hpw;
	 plink_haplotype_writer_t<> writer(hpw, "\t", "|");
	 ofstream os(hap_file);
	 if (!os.is_open()) {
		L_ERROR("Impossible to open haplotype result file '" << hap_file << "'.");
		throw logic_error(string("Impossible to open haplotype result file '")
								+ hap_file + "'.");
	 }
	 writer.write(os, ped);
	 os.close();
	 L_INFO("Haplotype configuration successfully saved.");
  };

  bool read_SAT_results(pedcnf_t& cnf,
								const string& result_file) const {
	 L_INFO("Reading SAT results from file '" << result_file << "'...");
	 ifstream is(result_file);
	 if (!is.is_open()) {
		L_ERROR("Impossible to open SAT result file '" << result_file << "'.");
		throw logic_error(string("Impossible to open SAT result file '")
								+ result_file + "'.");
	 }
	 const bool is_sat= cnf.assignment_from_minisat_format(is);
	 is.close();
	 L_INFO("SAT results successfully read from file '" << result_file << "'.");
	 return is_sat;
  };


public:

  void create_SAT_instance_from_pedigree(const string& ped_file,
													  const string& sat_file) const {
	 pedigree_t ped;
	 pedcnf_t cnf;
	 prepare_pedigree_and_sat(ped_file, ped, cnf);
// Output the instance
	 L_INFO("Saving SAT instance to file '" << sat_file << "'...");
	 ofstream out(sat_file);
	 if (!out.is_open()) {
		L_ERROR("Impossible to open file '" << sat_file
				<< "' for writing SAT instance.");
		throw logic_error(string("Impossible to open file '") + sat_file
								+ "' for writing SAT instance.");
	 }
	 const string headers[] = {
		"SAT instance",
		string("pedigree: ") + ped_file,
		string("sat: ") + sat_file,
		string("source version: ") + APPLICATION_SOURCE_VERSION
	 };
	 cnf.clauses_to_dimacs_format(out, vector<string>(headers, headers+4));
	 out.close();
	 L_INFO("SAT instance successfully saved to file '" << sat_file << "'.");
  };



  bool compute_HC_from_SAT_results(const string& ped_file,
											  const string& result_file,
											  const string& hap_file) const {
	 pedigree_t ped;
	 pedcnf_t cnf;
	 prepare_pedigree_and_sat(ped_file, ped, cnf);

// Open the result file and read the assignment
	 const bool is_sat= read_SAT_results(cnf, result_file);

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
		save_ZRHC(ped, hap_file);
	 } else {
		L_INFO("The pedigree CANNOT be realized by a zero-recombinant haplotype "
			  "configuration.");
// Do nothing
	 }
	 return is_sat;
  }


};
