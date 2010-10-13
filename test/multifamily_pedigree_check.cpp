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

#include "pedigree.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

TEST(multipedigree, creation) {
  multifamily_pedigree_t multip(4);
  multifamily_pedigree_t::pedigree_t& ped0= multip.add_family();
  multifamily_pedigree_t::pedigree_t& ped1= multip.add_family();
  ASSERT_EQ(&ped0, &multip.families()[0]);
  ASSERT_EQ(&ped1, &multip.families()[1]);
}
