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

#include <boost/static_assert.hpp>

#ifndef EXIT_NO_ZRHC
#define EXIT_NO_ZRHC (2)
#endif

BOOST_STATIC_ASSERT(EXIT_FAILURE != EXIT_NO_ZRHC);

using namespace std;


class zrhcstar_application_t: public application_t {

public:

  zrhcstar_application_t()
		:application_t(APPLICATION_CODENAME " " APPLICATION_VERSION_STRING)
  {}

private:

  typedef plink_reader_t<>::multifamily_pedigree_t pedigree_t;
  typedef pedigree_t::pedigree_t family_t;

  void prepare_pedigree_and_sat(const string& ped_file,
										  pedigree_t& mped,
										  pedcnf_t& cnf) const {
// Open the file and read the pedigree
	 ifstream is(ped_file);
	 if (!is.is_open()) {
		ERROR("Impossible to open pedigree file '" << ped_file << "'.");
		throw logic_error(string("Impossible to open pedigree file '")
								+ ped_file + "'.");
	 }

	 INFO("Reading pedigree from file '" << ped_file << "'...");
	 biallelic_genotype_reader_t<> gr;
	 plink_reader_t<> reader(gr);
	 reader.read(is, mped);
	 is.close();
	 INFO("Pedigree successfully read from file '" << ped_file << "'.");

	 if (mped.families().empty()) {
		throw logic_error(string("No family has been read from file '")
								+ ped_file + "'.");
	 }
	 if (mped.families().size() > 1) {
		throw logic_error(string("The pedigree read from file '")
								+ ped_file + "' has more than one family.");
	 }

// Prepare the SAT instance
	 INFO("Preparing SAT instance from pedigree...");
	 cnf= ped2cnf(mped.families().front());
	 INFO("SAT instance successfully prepared.");
  }

  void create_SAT_instance_from_pedigree(const string& ped_file,
													  const string& sat_file) const {
	 pedigree_t ped;
	 pedcnf_t cnf;
	 prepare_pedigree_and_sat(ped_file, ped, cnf);
// Output the instance
	 INFO("Saving SAT instance to file '" << sat_file << "'...");
	 ofstream out(sat_file);
	 if (!out.is_open()) {
		ERROR("Impossible to open file '" << sat_file
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
	 INFO("SAT instance successfully saved to file '" << sat_file << "'.");
  };



  void save_ZRHC(pedigree_t& ped,
					  const string& hap_file) const {
	 INFO("Saving haplotype configuration to file '" << hap_file << "'...");
// FIXME: Improve template instantiation
	 biallelic_haplotype_pair_writer_t<> hpw;
	 plink_haplotype_writer_t<> writer(hpw, "\t", "|");
	 ofstream os(hap_file);
	 if (!os.is_open()) {
		ERROR("Impossible to open haplotype result file '" << hap_file << "'.");
		throw logic_error(string("Impossible to open haplotype result file '")
								+ hap_file + "'.");
	 }
	 writer.write(os, ped);
	 os.close();
	 INFO("Haplotype configuration successfully saved.");
  };

  bool read_SAT_results(pedcnf_t& cnf,
								const string& result_file) const {
	 INFO("Reading SAT results from file '" << result_file << "'...");
	 ifstream is(result_file);
	 if (!is.is_open()) {
		ERROR("Impossible to open SAT result file '" << result_file << "'.");
		throw logic_error(string("Impossible to open SAT result file '")
								+ result_file + "'.");
	 }
	 const bool is_sat= cnf.assignment_from_minisat_format(is);
	 is.close();
	 INFO("SAT results successfully read from file '" << result_file << "'.");
	 return is_sat;
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
		INFO("The pedigree can be realized by a zero-recombinant haplotype "
			  "configuration.");
		pedigree_t::pedigree_t& family= ped.families().front();
// Compute the actual haplotype configuration
		compute_ZRHC_from_SAT(family, cnf);
// Check the haplotype configuration
		const bool ok=
		  family.is_consistent() &&
		  family.is_zero_recombinant();
		if (ok) {
		  INFO("The computed haplotype configuration is valid.");
		} else {
		  ERROR("The computed haplotype configuration is not valid.");
		  MY_FAIL;
		}
// Output the haplotype configuration
		save_ZRHC(ped, hap_file);
	 } else {
		INFO("The pedigree CANNOT be realized by a zero-recombinant haplotype "
			  "configuration.");
// Do nothing
	 }
	 return is_sat;
  }

protected:

  virtual po::options_description
  get_named_options() const {
	 po::options_description desc("Available options");
	 desc.add_options()
		("help,?", po::bool_switch(),
		 "Produce (this) help message.")
		("create,1", po::bool_switch(),
		 "Create the SAT instance from the pedigree file.")
		("read,2", po::bool_switch(),
		 "Read the results produced by the SAT solver.")
		("pedigree,p",
		 po::value< std::string >()->default_value("pedigree.ped"),
		 "File storing the genotyped pedigree.")
		("sat,s",
		 po::value< std::string >()->default_value("instance.cnf"),
		 "File storing the SAT instance.")
		("result,r",
		 po::value< std::string >()->default_value("sat-result.txt"),
		 "File storing the results produced by the SAT solver.")
		("haplotypes,h",
		 po::value< std::string >()->default_value("haplotypes.txt"),
		 "File storing the computed haplotype configuration.")
		;
	 return desc;
  };

  virtual int execution(int argc, char** argv,
								const po::variables_map& vm) {

// Generate the help message and exit
	 if (vm["help"].as<bool>()) {
		cout << _name << endl;
		cout << get_named_options() << endl;
		return EXIT_SUCCESS;
	 }

	 int main_ris= EXIT_SUCCESS;

// Check parameter values
	 DEBUG("Checking program parameters...");
	 mode_options(vm, "create", "read");
	 conflicting_options(vm, "create", "read");
	 option_dependency(vm, "create", "pedigree");
	 option_dependency(vm, "create", "sat");
	 option_dependency(vm, "read", "pedigree");
	 option_dependency(vm, "read", "result");
	 option_dependency(vm, "read", "haplotypes");

	 MY_ASSERT(vm["create"].as<bool>() != vm["read"].as<bool>());
	 DEBUG("Check completed.");

// Dispatch the work depending on the program parameters
	 if (vm["create"].as<bool>()) {
//    Creation of the SAT instance
		INFO("Creation of the SAT instance from the pedigree of file '"
			  << vm["pedigree"].as<string>() << "'...");

		create_SAT_instance_from_pedigree(vm["pedigree"].as<string>(),
													 vm["sat"].as<string>());
		INFO("SAT instance successfully created and saved.");
		main_ris= EXIT_SUCCESS;
	 } else if (vm["read"].as<bool>()) {
//    Computation of the haplotype configuration
		INFO("Computation of the haplotype configuration from the "
			  "pedigree of file '"
			  << vm["pedigree"].as<string>()
			  << "' and the results of the SAT solver stored in file '"
			  << vm["result"].as<string>() << "'...");
		bool is_zrhc= compute_HC_from_SAT_results(
											vm["pedigree"].as<string>(),
											vm["result"].as<string>(),
											vm["haplotypes"].as<string>());
		if (is_zrhc) {
		  INFO("Zero-Recombinant Haplotype Configuration successfully "
				 "computed and saved.");
		  main_ris= EXIT_SUCCESS;
		} else {
		  INFO("No Zero-Recombinant Haplotype Configuration can exist. "
				 "Exiting without haplotype configuration.");
		  main_ris= EXIT_NO_ZRHC;
		}
	 } else {
// We should not arrive here
		MY_FAIL;
		throw logic_error(string("Modes have not been recognized."));
	 }

	 return main_ris;
  }

};



int main(int argc, char** argv) {
  zrhcstar_application_t app;
  return app.execute(argc, argv);
}
