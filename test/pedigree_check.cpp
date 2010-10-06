#include <gtest/gtest.h>

#include "pedigree.hpp"

#include <iostream>
#include <sstream>
#include <string>

TEST(pedigree, access_hm) {
  pedigree_t ped(4);

  pedigree_t::individual_t& ind0= ped.add_individual();
  pedigree_t::individual_t& ind1= ped.add_individual();
  pedigree_t::individual_t& ind2= ped.add_individual();

  ind0.hm(0)= pedigree_t::h::ALLELE1;
  ind0.hm(1)= pedigree_t::h::ALLELE2;
  ind0.hm(2)= pedigree_t::h::MISS;
  ASSERT_EQ(ind0.hm(0), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hm(1), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hm(2), pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hm()[0], pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hm()[1], pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hm()[2], pedigree_t::h::MISS);
  ASSERT_EQ(ped.get_by_progr(0).hm(0), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ped.get_by_progr(0).hm(1), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ped.get_by_progr(0).hm(2), pedigree_t::h::MISS);

  ind0.hm()[0]= pedigree_t::h::MISS;
  ind0.hm()[1]= pedigree_t::h::ALLELE1;
  ind0.hm()[2]= pedigree_t::h::ALLELE2;
  ASSERT_EQ(ind0.hm(0), pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hm(1), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hm(2), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hm()[0], pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hm()[1], pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hm()[2], pedigree_t::h::ALLELE2);
  ASSERT_EQ(ped.get_by_progr(0).hm(0), pedigree_t::h::MISS);
  ASSERT_EQ(ped.get_by_progr(0).hm(1), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ped.get_by_progr(0).hm(2), pedigree_t::h::ALLELE2);

}

TEST(pedigree, access_hp) {
  pedigree_t ped(4);

  pedigree_t::individual_t& ind0= ped.add_individual();
  pedigree_t::individual_t& ind1= ped.add_individual();
  pedigree_t::individual_t& ind2= ped.add_individual();

  ind0.hp(0)= pedigree_t::h::ALLELE1;
  ind0.hp(1)= pedigree_t::h::ALLELE2;
  ind0.hp(2)= pedigree_t::h::MISS;
  ASSERT_EQ(ind0.hp(0), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hp(1), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hp(2), pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hp()[0], pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hp()[1], pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hp()[2], pedigree_t::h::MISS);
  ASSERT_EQ(ped.get_by_progr(0).hp(0), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ped.get_by_progr(0).hp(1), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ped.get_by_progr(0).hp(2), pedigree_t::h::MISS);

  ind0.hp()[0]= pedigree_t::h::MISS;
  ind0.hp()[1]= pedigree_t::h::ALLELE1;
  ind0.hp()[2]= pedigree_t::h::ALLELE2;
  ASSERT_EQ(ind0.hp(0), pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hp(1), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hp(2), pedigree_t::h::ALLELE2);
  ASSERT_EQ(ind0.hp()[0], pedigree_t::h::MISS);
  ASSERT_EQ(ind0.hp()[1], pedigree_t::h::ALLELE1);
  ASSERT_EQ(ind0.hp()[2], pedigree_t::h::ALLELE2);
  ASSERT_EQ(ped.get_by_progr(0).hp(0), pedigree_t::h::MISS);
  ASSERT_EQ(ped.get_by_progr(0).hp(1), pedigree_t::h::ALLELE1);
  ASSERT_EQ(ped.get_by_progr(0).hp(2), pedigree_t::h::ALLELE2);

}

TEST(pedigree, access_g) {
  pedigree_t ped(4);

  pedigree_t::individual_t& ind0= ped.add_individual();
  pedigree_t::individual_t& ind1= ped.add_individual();
  pedigree_t::individual_t& ind2= ped.add_individual();

  ind0.g(0)= pedigree_t::g::HETER;
  ind0.g(1)= pedigree_t::g::HOMO1;
  ind0.g(2)= pedigree_t::g::MISS;
  ind0.g(3)= pedigree_t::g::HOMO2;
  ASSERT_EQ(ind0.g(0), pedigree_t::g::HETER);
  ASSERT_EQ(ind0.g(1), pedigree_t::g::HOMO1);
  ASSERT_EQ(ind0.g(2), pedigree_t::g::MISS);
  ASSERT_EQ(ind0.g(3), pedigree_t::g::HOMO2);
  ASSERT_EQ(ind0.g()[0], pedigree_t::g::HETER);
  ASSERT_EQ(ind0.g()[1], pedigree_t::g::HOMO1);
  ASSERT_EQ(ind0.g()[2], pedigree_t::g::MISS);
  ASSERT_EQ(ind0.g()[3], pedigree_t::g::HOMO2);
  ASSERT_EQ(ped.get_by_progr(0).g(0), pedigree_t::g::HETER);
  ASSERT_EQ(ped.get_by_progr(0).g(1), pedigree_t::g::HOMO1);
  ASSERT_EQ(ped.get_by_progr(0).g(2), pedigree_t::g::MISS);
  ASSERT_EQ(ped.get_by_progr(0).g(3), pedigree_t::g::HOMO2);

}

TEST(pedigree, identifiers) {
  pedigree_t ped(4);

  pedigree_t::individual_t& ind0= ped.add_individual();
  pedigree_t::individual_t& ind1= ped.add_individual();
  pedigree_t::individual_t& ind2= ped.add_individual();
  ind0.set_id(100);
  ind1.set_id(101);

  ASSERT_THROW(ind2.set_id(101), assertion_failed_exception);

  ind2.set_id(102);

  ASSERT_EQ(ind0.id(), 100);
  ASSERT_EQ(ind1.id(), 101);
  ASSERT_EQ(ind2.id(), 102);

  ind0.set_id(pedigree_t::not_existent_id);
  ind1.set_id(100);
  ind1.set_id(101);
  ind0.set_id(100);

  ASSERT_EQ(ind0.id(), 100);
  ASSERT_EQ(ind1.id(), 101);
  ASSERT_EQ(ind2.id(), 102);

  ASSERT_NO_THROW(ped.get_by_progr(2));
  ASSERT_THROW(ped.get_by_progr(3), assertion_failed_exception);

  ASSERT_NO_THROW(ped.get_by_id(100));
  ASSERT_NO_THROW(ped.get_by_id(101));
  ASSERT_NO_THROW(ped.get_by_id(102));

  ind0.set_id(pedigree_t::not_existent_id);

  ASSERT_THROW(ped.get_by_id(pedigree_t::not_existent_id),
					assertion_failed_exception);
  ASSERT_THROW(ped.get_by_id(100),
					assertion_failed_exception);
  ASSERT_THROW(ped[1000],
					assertion_failed_exception);

}


TEST(pedigree, trios) {
  pedigree_t ped(4);

  pedigree_t::individual_t& ind0= ped.add_individual();
  pedigree_t::individual_t& ind1= ped.add_individual();
  pedigree_t::individual_t& ind2= ped.add_individual();
  pedigree_t::individual_t& ind3= ped.add_individual();
  pedigree_t::individual_t& ind4= ped.add_individual();
  pedigree_t::individual_t& ind5= ped.add_individual();
  pedigree_t::individual_t& ind6= ped.add_individual();
  pedigree_t::individual_t& ind7= ped.add_individual();
  ind0.set_id(100);
  ind1.set_id(101);
  ind2.set_id(102);
  ind3.set_id(103);
  ind4.set_id(104);
  ind5.set_id(105);
  ind6.set_id(106);
  ind7.set_id(107);

  ASSERT_NO_THROW(ind1.set_gender(gender_t::FEMALE));
  ASSERT_NO_THROW(ind1.set_gender(gender_t::MALE));
  ASSERT_EQ(ind1.gender(), gender_t::MALE);

  ped.add_trio_by_id(100, 101, 102);

  ASSERT_THROW(ind2.set_gender(gender_t::MALE), assertion_failed_exception);
  ASSERT_THROW(ind1.set_gender(gender_t::UNSPEC), assertion_failed_exception);

  ASSERT_EQ(ind0.gender(), gender_t::UNSPEC);
  ASSERT_EQ(ind1.gender(), gender_t::MALE);
  ASSERT_EQ(ind2.gender(), gender_t::FEMALE);

  ASSERT_THROW(ped.add_trio_by_progr(0, 1, 3),
					assertion_failed_exception);

  ASSERT_NO_THROW(ped.add_trio_by_progr(3, 1, 2));
  ASSERT_NO_THROW(ped.add_trio_by_id(105, 101, 104));
  ASSERT_THROW(ped.add_trio_by_id(106, 104, 102),
					assertion_failed_exception);
  ASSERT_NO_THROW(ped.add_trio_by_id(106, 100, 103));

  ASSERT_EQ(ind0.gender(), gender_t::MALE);
  ASSERT_EQ(ind1.gender(), gender_t::MALE);
  ASSERT_EQ(ind2.gender(), gender_t::FEMALE);
  ASSERT_EQ(ind3.gender(), gender_t::FEMALE);
  ASSERT_EQ(ind4.gender(), gender_t::FEMALE);
  ASSERT_EQ(ind5.gender(), gender_t::UNSPEC);
  ASSERT_EQ(ind6.gender(), gender_t::UNSPEC);

  ASSERT_NO_THROW(ped.add_trio_by_id(101,
												 pedigree_t::not_existent_id,
												 pedigree_t::not_existent_id));

  ASSERT_TRUE(ind1.is_founder());

  ASSERT_NO_THROW(ped.add_trio_by_id(101,
												 pedigree_t::not_existent_id,
												 107));
  ASSERT_EQ(ind7.gender(), gender_t::FEMALE);

  ASSERT_FALSE(ind1.is_founder());
  ASSERT_TRUE(ind1.has_mother());
  ASSERT_FALSE(ind1.has_father());
  ASSERT_TRUE(ind1.has_children());
  ASSERT_FALSE(ind5.has_children());
  ASSERT_FALSE(ind5.is_founder());
}
