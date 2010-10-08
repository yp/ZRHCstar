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
