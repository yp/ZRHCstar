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

#define PEDIGREE_STR															  \
  "0 1 0 0 1 phenotype 1 1 2 2 2 2 2 2 1 1\n"						  \
  "0 2 0 0 2 phenotype 2 2 1 1 1 1 1 1 1 1\n"						  \
  "0 3 1 2 2 phenotype 1 2 0 0 1 2 1 2 1 1\n"						  \
  "0 4 0 0 1 phenotype 1 2 1 2 1 1 1 1 0 0\n"						  \
  "0 5 4 3 1 phenotype 1 2 1 2 0 0 1 1 1 2\n"

#define SAT_INSTANCE_STR													\
  "c SAT instance\n"															\
  "c\n"																			\
  "c v     1 h_2_0\n"														\
  "c v     2 h_2_1\n"														\
  "c v     3 h_2_2\n"														\
  "c v     4 h_2_3\n"														\
  "c v     5 w_2_1\n"														\
  "c v     6 h_3_0\n"														\
  "c v     7 s_3_4\n"														\
  "c v     8 h_4_0\n"														\
  "c v     9 h_3_1\n"														\
  "c v    10 h_4_1\n"														\
  "c v    11 h_4_2\n"														\
  "c v    12 h_3_4\n"														\
  "c v    13 w_3_4\n"														\
  "c v    14 h_4_4\n"														\
  "c v    15 dummy_7_13\n"													\
  "c v    16 s_2_4\n"														\
  "c v    17 dummy_16_5\n"													\
  "c v    18 w_4_2\n"														\
  "c\n"																			\
  "p cnf 18 44\n"																\
  "   -18    -16    -11      3      0\n"								\
  "   -18    -16     -3     11      0\n"								\
  "   -18    -11     -3     16      0\n"								\
  "   -18      3     11     16      0\n"								\
  "   -17    -10      2      0\n"										\
  "   -17     -2     10      0\n"										\
  "   -17      5      0\n"													\
  "   -17     16      0\n"													\
  "   -16    -11     -3     18      0\n"								\
  "   -16     -8      1      0\n"										\
  "   -16     -5     17      0\n"										\
  "   -16     -1      8      0\n"										\
  "   -16      3     11     18      0\n"								\
  "   -16      4      0\n"													\
  "   -15    -14    -12      0\n"										\
  "   -15      7      0\n"													\
  "   -15     12     14      0\n"										\
  "   -15     13      0\n"													\
  "   -14     12     15      0\n"										\
  "   -13     -7     15      0\n"										\
  "   -12     14     15      0\n"										\
  "   -11      0\n"															\
  "   -11      3     16     18      0\n"								\
  "   -10     -9     -7      0\n"										\
  "   -10     -2     17      0\n"										\
  "   -10      7      9      0\n"										\
  "    -9      7     10      0\n"										\
  "    -8     -7     -6      0\n"										\
  "    -8     -1     16      0\n"										\
  "    -8      6      7      0\n"										\
  "    -7      6      8      0\n"										\
  "    -7      9     10      0\n"										\
  "    -6      7      8      0\n"										\
  "    -5      2      0\n"													\
  "    -4     16      0\n"													\
  "    -3     11     16     18      0\n"								\
  "    -2      5      0\n"													\
  "    -1      0\n"															\
  "     1      8     16      0\n"										\
  "     2      0\n"															\
  "     2     10     17      0\n"										\
  "     3      0\n"															\
  "     4      0\n"															\
  "    14      0\n"

#define SAT_RESULTS_STR													\
  "SAT\n"																	\
  "-1 2 3 4 5 6 7 -8 -9 10 -11 12 -13 14 -15 16 17 -18 0"

#define HC_STR																 \
  "0	1	0	0	1	phenotype	1|1	2|2	2|2	2|2	1|1\n" \
  "0	2	0	0	2	phenotype	2|2	1|1	1|1	1|1	1|1\n" \
  "0	3	1	2	2	phenotype	1|2	2|1	2|1	2|1	1|1\n" \
  "0	4	0	0	1	phenotype	2|1	1|2	1|1	1|1	2|2\n" \
  "0	5	4	3	1	phenotype	1|2	2|1	1|1	1|1	2|1\n"

TEST(zrhc, pedtocnf) {
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


TEST(zrhc, sattohc) {
  string pedigree(PEDIGREE_STR);
  string sat_results(SAT_RESULTS_STR);
  string hc(HC_STR);

  istringstream is_ped(pedigree);
  istringstream is_res(sat_results);
  ostringstream os;

  zrhcstar_t zrhcstar;

  const bool is_sat=
	 zrhcstar.compute_HC_from_SAT_results(is_ped, is_res, os);

  ASSERT_TRUE( is_sat );
  ASSERT_EQ( hc, os.str());

}
