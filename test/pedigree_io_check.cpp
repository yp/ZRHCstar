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

#include "io-pedigree.hpp"

#include <iostream>
#include <sstream>
#include <string>

TEST(pedigree, input) {
  std::string
    str(
      "# comment line\n"
      "#\n"
      "0 1 2 3 1 phenotype 1 1 2 2 1 2\n"
      "0 2 4 5 1 phenotype 1 1 2 2 1 2\n"
      "0 3 6 7 2 phenotype 1 1 2 2 1 2\n"
      "0 5 10 11 2 phenotype 1 1 2 2 1 2\n"
      "0 4 8 9 1 phenotype 1 1 2 2 1 2\n"
      "0 10 12 13 1 phenotype 1 1 2 2 0 0\n"
      "0 12 0 0 1 phenotype 1 1 2 2 0 0\n"
      );
  std::istringstream is(str);
  biallelic_genotype_reader_t<> gr;
  plink_reader_t<> reader(gr);
  plink_reader_t<>::multifamily_pedigree_t ped;
  reader.read(is, ped);

  ASSERT_FALSE(ped[0][1].is_founder());
  ASSERT_FALSE(ped[0][2].is_founder());
  ASSERT_FALSE(ped[0][3].is_founder());
  ASSERT_FALSE(ped[0][4].is_founder());
  ASSERT_FALSE(ped[0][5].is_founder());
  ASSERT_TRUE(ped[0][6].is_founder());
  ASSERT_TRUE(ped[0][7].is_founder());
  ASSERT_TRUE(ped[0][8].is_founder());
  ASSERT_TRUE(ped[0][9].is_founder());
  ASSERT_FALSE(ped[0][10].is_founder());
  ASSERT_TRUE(ped[0][11].is_founder());
  ASSERT_TRUE(ped[0][12].is_founder());
  ASSERT_TRUE(ped[0][13].is_founder());

  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::HOMO1,
            ped[0][1].g(0));
  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::HOMO2,
            ped[0][1].g(1));
  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::HETER,
            ped[0][1].g(2));

  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::HOMO1,
            ped[0][12].g(0));
  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::HOMO2,
            ped[0][12].g(1));
  ASSERT_EQ(plink_reader_t<>::multifamily_pedigree_t::pedigree_t::g::MISS,
            ped[0][12].g(2));

}


TEST(pedigree, read_eq_write) {
  std::string
    str(
        "0 1 0 0 1 phenotype 1 1 2 2 1 2\n"
        "0 2 0 0 2 phenotype 1 1 2 2 1 2\n"
        "0 3 1 2 1 phenotype 1 1 2 2 1 2\n"
        "0 4 1 2 2 phenotype 1 1 2 2 1 2\n"
        "0 5 3 4 1 phenotype 1 1 2 2 1 2\n"
        "1 1 0 0 1 phenotype 1 1 2 2 1 2\n"
        "1 2 0 0 2 phenotype 1 1 2 2 1 2\n"
        "1 3 1 2 1 phenotype 1 1 2 2 1 2\n"
      );
  std::istringstream is(str);
  biallelic_genotype_reader_t<> gr;
  plink_reader_t<> reader(gr);
  plink_reader_t<>::multifamily_pedigree_t ped;
  reader.read(is, ped);

  biallelic_genotype_writer_t<> gw;
  plink_writer_t<> writer(gw, " ");
  std::ostringstream os;
  writer.write(os, ped);

  ASSERT_EQ(str, os.str());
}


TEST(pedigree, write_haplotypes) {
  biallelic_haplotype_pair_writer_t<> hpw;
  plink_haplotype_writer_t<> writer(hpw, " ", "|");
  plink_haplotype_writer_t<>::multifamily_pedigree_t mped(3);
  plink_haplotype_writer_t<>::multifamily_pedigree_t::pedigree_t& ped= mped.add_family();

  pedigree_t::individual_t& ind0= ped.add_individual(1);
  pedigree_t::individual_t& ind1= ped.add_individual(2);
  pedigree_t::individual_t& ind2= ped.add_individual(3);
  ind0.phenotype()= "phenotype1";
  ind1.phenotype()= "phenotype2";
  ind2.phenotype()= "phenotype3";
  ped.add_trio_by_id(1, 2, 3);

  ind0.hp(0)= pedigree_t::h::ALLELE1;
  ind0.hp(1)= pedigree_t::h::ALLELE2;
  ind0.hp(2)= pedigree_t::h::MISS;

  ind0.hm(0)= pedigree_t::h::ALLELE2;
  ind0.hm(1)= pedigree_t::h::ALLELE1;
  ind0.hm(2)= pedigree_t::h::MISS;

  ind1.hp(0)= pedigree_t::h::MISS;
  ind1.hp(1)= pedigree_t::h::ALLELE2;
  ind1.hp(2)= pedigree_t::h::ALLELE2;

  ind1.hm(0)= pedigree_t::h::MISS;
  ind1.hm(1)= pedigree_t::h::ALLELE2;
  ind1.hm(2)= pedigree_t::h::ALLELE2;

  ind2.hp(0)= pedigree_t::h::MISS;
  ind2.hp(1)= pedigree_t::h::ALLELE1;
  ind2.hp(2)= pedigree_t::h::ALLELE2;

  ind2.hm(0)= pedigree_t::h::MISS;
  ind2.hm(1)= pedigree_t::h::ALLELE1;
  ind2.hm(2)= pedigree_t::h::ALLELE2;

  std::ostringstream os;
  writer.write(os, mped);

  std::string is("0 3 0 0 2 phenotype3 0|0 1|1 2|2\n"
					  "0 2 0 0 1 phenotype2 0|0 2|2 2|2\n"
					  "0 1 2 3 0 phenotype1 1|2 2|1 0|0\n");

  ASSERT_EQ(is, os.str());

}
