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
#include "zrhc_app.hpp"

#include "configuration.h"

#include "application.hpp"
#include "log.hpp"
#include "assertion.hpp"
#include "utility.hpp"

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

  ifstream& get_ifstream(const string& file_name, ifstream& is) const {
	 DEBUG("Opening file '" << file_name << "' for reading...");
	 is.open(file_name);
	 if (!is.is_open()) {
		ERROR("Impossible to open file '" << file_name << "' for reading.");
		throw logic_error(string("Impossible to open file '")
								+ file_name + "' for reading.");
	 }
	 DEBUG("File '" << file_name << "' successfully opened.");
	 return is;
  };

  ofstream& get_ofstream(const string& file_name, ofstream& os) const {
	 DEBUG("Opening file '" << file_name << "' for writing...");
	 os.open(file_name);
	 if (!os.is_open()) {
		ERROR("Impossible to open file '" << file_name << "' for writing.");
		throw logic_error(string("Impossible to open file '")
								+ file_name + "' for writing.");
	 }
	 DEBUG("File '" << file_name << "' successfully opened.");
	 return os;
  };

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

	 zrhcstar_t zrhcstar;

// Dispatch the work depending on the program parameters
	 if (vm["create"].as<bool>()) {
//    Creation of the SAT instance
		INFO("Creation of the SAT instance from the pedigree of file '"
			  << vm["pedigree"].as<string>() << "'...");

		ifstream ped_is;
		get_ifstream(vm["pedigree"].as<string>(), ped_is);
		ofstream sat_os;
		get_ofstream(vm["sat"].as<string>(), sat_os);
		const std::string headers[] = {
		  "SAT instance",
		  std::string("pedigree: ") + vm["pedigree"].as<string>(),
		  std::string("sat: ") + vm["sat"].as<string>(),
		  std::string("source version: ") + APPLICATION_SOURCE_VERSION
		};
		zrhcstar.create_SAT_instance_from_pedigree(ped_is,
																 sat_os,
																 vector<string>(headers,
																					 headers+4));
		ped_is.close();
		sat_os.close();

		INFO("SAT instance successfully created and saved.");

		main_ris= EXIT_SUCCESS;

	 } else if (vm["read"].as<bool>()) {
//    Computation of the haplotype configuration
		INFO("Computation of the haplotype configuration from the "
			  "pedigree of file '"
			  << vm["pedigree"].as<string>()
			  << "' and the results of the SAT solver stored in file '"
			  << vm["result"].as<string>() << "'...");
		ifstream ped_is;
		get_ifstream(vm["pedigree"].as<string>(), ped_is);
		ifstream res_is;
		get_ifstream(vm["result"].as<string>(), res_is);
		ofstream hap_os;
		get_ofstream(vm["haplotypes"].as<string>(), hap_os);
		bool is_zrhc=
		  zrhcstar.compute_HC_from_SAT_results(ped_is, res_is, hap_os);
		ped_is.close();
		res_is.close();
		hap_os.close();
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
