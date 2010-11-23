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

#include "haplotypes_genotypes.hpp"
#include "io-haplotypes_genotypes.hpp"

#include <iostream>
#include <sstream>
#include <string>

TEST(genotype, io) {

  genotype_t g(4);

  std::string is= "2105";
  std::istringstream in(is);
  in >> g;

  std::ostringstream out;
  out << g;

  ASSERT_EQ( is, out.str());

}

TEST(genotype, io_fail) {

  genotype_t g(4);

  std::string is= "2135";
  std::istringstream in(is);
  ASSERT_THROW({ in >> g; }, assertion_failed_exception);
}

TEST(genotype, limits) {

  genotype_t g(4);

  ASSERT_THROW({ g[4] == genotype_t::base::HOMO1; },
					assertion_failed_exception);
}

TEST(genotype, access) {

  std::string is= "2105";
  genotype_t g(4);

  g[1]= genotype_t::base::HOMO2;
  g[0]= genotype_t::base::HETER;
  g[3]= genotype_t::base::MISS;
  g[2]= genotype_t::base::HOMO1;

  std::ostringstream out;
  out << g;

  ASSERT_EQ( is, out.str());

  ASSERT_EQ( g[0], genotype_t::base::HETER );
  ASSERT_EQ( g[1], genotype_t::base::HOMO2 );
  ASSERT_EQ( g[2], genotype_t::base::HOMO1 );
  ASSERT_EQ( g[3], genotype_t::base::MISS  );

}


TEST(haplotype, io) {

  haplotype_t h(4);

  std::string is= "1020";
  std::istringstream in(is);
  in >> h;

  std::ostringstream out;
  out << h;

  ASSERT_EQ( is, out.str());

}

TEST(haplotype, io_fail) {

  haplotype_t h(4);

  std::string is= "1205";
  std::istringstream in(is);
  ASSERT_THROW({ in >> h; }, assertion_failed_exception);
}

TEST(haplotype, limits) {

  haplotype_t h(4);

  ASSERT_THROW({ h[10] == haplotype_t::base::ALLELE2; },
					assertion_failed_exception);
}

TEST(haplotype, access) {

  std::string is= "021";
  haplotype_t h(3);

  h[1]= haplotype_t::base::ALLELE2;
  h[2]= haplotype_t::base::ALLELE1;
  h[0]= haplotype_t::base::MISS;

  std::ostringstream out;
  out << h;

  ASSERT_EQ( is, out.str());

  ASSERT_EQ( h[0], haplotype_t::base::MISS );
  ASSERT_EQ( h[1], haplotype_t::base::ALLELE2 );
  ASSERT_EQ( h[2], haplotype_t::base::ALLELE1 );

}

TEST(haplotype, writer) {

  std::string is= "0 2 1";

  typedef generic_fixlen_vector_t< single_biallelic_haplotype_t< 1, 2, 0 > > my_haplotype_t;

  my_haplotype_t h(3);

  h[0]= my_haplotype_t::base::MISS;
  h[1]= my_haplotype_t::base::ALLELE2;
  h[2]= my_haplotype_t::base::ALLELE1;

  std::ostringstream out;
  biallelic_haplotype_writer_t<1,2,0> bw;
  bw.encode(out, h);

  ASSERT_EQ( is, out.str() );

  ASSERT_EQ( h[0], my_haplotype_t::base::MISS );
  ASSERT_EQ( h[1], my_haplotype_t::base::ALLELE2 );
  ASSERT_EQ( h[2], my_haplotype_t::base::ALLELE1 );

}


TEST(haplotype, pair_writer) {

  typedef generic_fixlen_vector_t< single_biallelic_haplotype_t< 1, 2, 0 > > my_haplotype_t;

  my_haplotype_t h1(9);
  my_haplotype_t h2(9);

  size_t i= 0;
  for (size_t i1= 0; i1 < my_haplotype_t::base::N_VALUES; ++i1) {
	 for (size_t i2= 0; i2 < my_haplotype_t::base::N_VALUES; ++i2, ++i) {
		h1[i]= my_haplotype_t::base::enum_values[i1];
		h2[i]= my_haplotype_t::base::enum_values[i2];
	 }
  }

  {
	 std::string is= "1|1 1|2 1|0 2|1 2|2 2|0 0|1 0|2 0|0";

	 std::ostringstream out;
	 biallelic_haplotype_pair_writer_t<1,2,0> bw;
	 bw.encode(out, h1, h2);

	 ASSERT_EQ( is, out.str() );
  }

  {
	 std::string is= "1%1-1%2-1%0-2%1-2%2-2%0-0%1-0%2-0%0";

	 std::ostringstream out;
	 biallelic_haplotype_pair_writer_t<1,2,0> bw;
	 bw.encode(out, h1, h2, "-", "%");

	 ASSERT_EQ( is, out.str() );
  }

}
