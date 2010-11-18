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

#include <iostream>

#include "io-pedigree.hpp"
#include "ped2cnf.hpp"

using namespace std;

class zrhcstar_application_t: public application_t {

public:

  zrhcstar_application_t()
		:application_t(APPLICATION_CODENAME " " APPLICATION_VERSION_STRING)
  {}

protected:

  virtual int execution(int argc, char** argv,
								const po::variables_map& vm) {
	 std::string
		str(
			 "0 1 0 0 1 phenotype 1 1 2 2 2 2 2 2 1 1\n"
			 "0 2 0 0 2 phenotype 2 2 1 1 1 1 1 1 1 1\n"
			 "0 3 1 2 2 phenotype 1 2 0 0 1 2 1 2 1 1\n"
			 "0 4 0 0 1 phenotype 1 2 1 2 1 1 1 1 0 0\n"
			 "0 5 4 3 1 phenotype 1 2 1 2 0 0 1 1 1 2\n"
			 // "1 1 0 0 1 phenotype 1 1 2 2 2 2 2 2 1 1\n"
			 // "1 2 0 0 2 phenotype 2 2 1 1 1 1 1 1 1 1\n"
			 // "1 3 1 2 2 phenotype 1 2 0 0 1 2 1 2 1 1\n"
			 // "1 4 0 0 1 phenotype 1 2 1 2 1 1 1 1 0 0\n"
			 // "1 5 4 3 1 phenotype 1 2 1 2 0 0 1 1 1 2\n"
			 );
	 std::istringstream is(str);

	 biallelic_genotype_reader_t<> gr;
	 plink_reader_t<> reader(gr);
	 plink_reader_t<>::multifamily_pedigree_t ped;
	 reader.read(is, ped);

	 size_t i= 0;

	 for (plink_reader_t<>::multifamily_pedigree_t::type::const_iterator it= ped.families().begin();
			it != ped.families().end();
			++it, ++i) {
// Prepare the SAT instance
		pedcnf_t cnf= ped2cnf(ped.families().front());

// Output the instance
		std::cout << "c === Family " << i << std::endl;
		cnf.clauses_to_dimacs_format(std::cout);
	 }


	 return EXIT_SUCCESS;
  }

};



int main(int argc, char** argv) {
  zrhcstar_application_t app;
  return app.execute(argc, argv);
}
