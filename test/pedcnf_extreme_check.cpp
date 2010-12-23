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
#include <gtest/gtest.h>

#include "zrhc_app.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define PEDIGREE_STR												  \
  "0 1 0 0 1 phenotype 1 1 1 1 1 1\n"						  \
  "0 2 0 0 2 phenotype 1 2 0 0 1 2\n"						  \
  "0 3 1 2 2 phenotype 1 1 1 1 1 1\n"

#define SAT_INSTANCE_STR								\
  "c SAT instance\n"										\
  "c\n"														\
  "c v     1 h_1_0\n"									\
  "c v     2 s_1_2\n"									\
  "c v     3 h_1_1\n"									\
  "c v     4 w_1_1\n"									\
  "c v     5 dummy_2_4\n"								\
  "c v     6 h_1_2\n"									\
  "c\n"														\
  "p cnf 6 9\n"											\
  "    -6      2      0\n"								\
  "    -5      2      0\n"								\
  "    -5      3      0\n"								\
  "    -5      4      0\n"								\
  "    -4     -2      5      0\n"					\
  "    -3      5      0\n"								\
  "    -2      1      0\n"								\
  "    -2      6      0\n"								\
  "    -1      2      0\n"

#define SAT_RESULTS_STR									\
  "SAT\n"													\
  "1 2 3 4 5 6 0"

#define HC_STR \
  "0	1	0	0	1	phenotype	1|1	1|1	1|1\n"	\
  "0	2	0	0	2	phenotype	2|1	2|1	2|1\n"	\
  "0	3	1	2	2	phenotype	1|1	1|1	1|1\n"

TEST(zrhc, ext_pedtocnf) {
  string pedigree(PEDIGREE_STR);
  string sat_instance(SAT_INSTANCE_STR);

  istringstream is(pedigree);
  ostringstream os;

  zrhcstar_t zrhcstar;

  zrhcstar.create_SAT_instance_from_pedigree(is,
															os,
															vector<string>(1,
																				"SAT instance"));

  ASSERT_EQ(sat_instance, os.str());

}

TEST(zrhc, ext_sattohc) {
  string pedigree(PEDIGREE_STR);
  string sat_results(SAT_RESULTS_STR);
  string hc(HC_STR);

  istringstream is_ped(pedigree);
  istringstream is_res(sat_results);
  ostringstream os;

  zrhcstar_t zrhcstar;

  zrhcstar_t::pedigree_t ped;
  pedcnf_t* cnf;
  const bool is_sat=
	 zrhcstar.compute_HC_from_SAT_results(is_ped, is_res, ped, cnf);
  delete cnf;

  ASSERT_TRUE( is_sat );
  zrhcstar_t::pedigree_t::pedigree_t& fam= ped.families().front();
  ASSERT_TRUE( fam.is_completely_haplotyped() );
  ASSERT_TRUE( fam.is_consistent() );
  ASSERT_TRUE( fam.is_zero_recombinant() );

  zrhcstar.save_ZRHC(ped, os);

  ASSERT_EQ( hc, os.str());

}

