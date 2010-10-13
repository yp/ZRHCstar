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

  std::string is= "0515";
  std::istringstream in(is);
  in >> h;

  std::ostringstream out;
  out << h;

  ASSERT_EQ( is, out.str());

}

TEST(haplotype, io_fail) {

  haplotype_t h(4);

  std::string is= "0152";
  std::istringstream in(is);
  ASSERT_THROW({ in >> h; }, assertion_failed_exception);
}

TEST(haplotype, limits) {

  haplotype_t h(4);

  ASSERT_THROW({ h[10] == haplotype_t::base::ALLELE2; },
					assertion_failed_exception);
}

TEST(haplotype, access) {

  std::string is= "510";
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


