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
/**
 *
 * in-pedigree.hpp
 *
 * Classes to represent readers of genotyped and haplotyped
 * pedigrees.
 *
 **/

#ifndef __IN_PEDIGREE_HPP__
#define __IN_PEDIGREE_HPP__

#include "haplotypes_genotypes.hpp"
#include "io-haplotypes_genotypes.hpp"
#include "pedigree.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "descriptive_class.hpp"

template <
  typename T_GENOTYPE= genotype_t,
  typename T_HAPLOTYPE= haplotype_t,
  typename T_PHENOTYPE= std::string,
  typename T_ID= size_t,
  typename T_VECTOR= boost::ptr_vector<basic_pedigree_t<T_GENOTYPE,
                                                        T_HAPLOTYPE,
                                                        T_PHENOTYPE,
                                                        T_ID>
                                       >
  >
class basic_in_multifamily_pedigree_t
  :public descriptive_class,
   public log_able_t< basic_in_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                                      T_PHENOTYPE, T_ID,
                                                      T_VECTOR>
                      >
{
public:
  typedef basic_multifamily_pedigree_t<T_GENOTYPE,
                                       T_HAPLOTYPE,
                                       T_PHENOTYPE,
                                       T_ID,
                                       T_VECTOR> multifamily_pedigree_t;

protected:

  virtual void read_impl(std::istream& in,
                         multifamily_pedigree_t& ped) const = 0;

public:

  void read(std::istream& in, multifamily_pedigree_t& ped) const {
    L_DEBUG("Reading multifamily pedigree with " << get_brief_description()
            << " reader...");
    read_impl(in, ped);
  };

};

/**
 * This class reads a sequence of families with different
 * identifiers from the same file
 * Example:
 * ID_FAMILY1 FAMILY1 ...
 * ID_FAMILY1 FAMILY1 ...
 * ID_FAMILY1 FAMILY1 ...
 * ID_FAMILY2 FAMILY2 ...
 * ID_FAMILY2 FAMILY2 ...
 * ID_FAMILY2 FAMILY2 ...
 * ID_FAMILY2 FAMILY2 ...
 * ID_FAMILY2 FAMILY2 ...
 * ID_FAMILY3 FAMILY3 ...
 * ID_FAMILY3 FAMILY3 ...
 * ID_FAMILY3 FAMILY3 ...
 * ID_FAMILY3 FAMILY3 ...
 **/

template <
  typename T_GENOTYPE= genotype_t,
  typename T_HAPLOTYPE= haplotype_t,
  typename T_PHENOTYPE= std::string,
  typename T_ID= size_t,
  typename T_VECTOR= boost::ptr_vector<basic_pedigree_t<T_GENOTYPE,
                                                        T_HAPLOTYPE,
                                                        T_PHENOTYPE,
                                                        T_ID>
                                       >
  >
class plink_reader_t
  :public basic_in_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                          T_PHENOTYPE, T_ID, T_VECTOR>
{

private:
  typedef basic_in_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                          T_PHENOTYPE, T_ID, T_VECTOR> base_t;

  class invalid_line_t {
  public:
    const std::string msg;
    invalid_line_t(const std::string& _msg= "unspecified error")
      :msg(_msg)
    {}
  };

  basic_genotype_reader_t<typename T_GENOTYPE::base>& _gr;

public:
  plink_reader_t(basic_genotype_reader_t<typename T_GENOTYPE::base>& gr)
    :_gr(gr)
  {}


protected:

  virtual const std::string get_brief_description_impl() const {
    return "plink-fmt";
  }

  virtual const std::string get_long_description_impl() const {
    return "PLINK format";
  }

  virtual const std::string get_format_impl() const {
    return "plink";
  }

  template <typename _Field_Type>
  void read_field(std::istream& in,
                  _Field_Type& field,
                  const std::string& description)
    const
    throw (invalid_line_t) {

    bool ris= (in >> field);
    if (ris) {
      L_TRACE(description << ": >" << field << "<");
    } else {
      std::string trim_desc(description);
      boost::trim(trim_desc);
      throw invalid_line_t("wrong \""+trim_desc+"\"");
    }
  }

  virtual void read_impl(std::istream& in, multifamily_pedigree_t& ped) const {
    std::string buff;
    std::string prev_family_id= "";
    while (std::getline(in, buff)) {
      L_TRACE("Read line starting with >" << buff.substr(0, 30) << "<");
      boost::trim(buff);
      if ((buff.length()==0) || boost::starts_with(buff, "#")) {
        L_TRACE("Comment or empty line ignored.");
      } else {
        try {
          // Process line
          std::istringstream is(buff);
          std::string new_fam_id;
          T_ID new_ind_id;
          T_ID new_fat_id;
          T_ID new_mot_id;
          gender_t gender;
          T_PHENOTYPE pheno;
          std::string rest;
          read_field(is, new_fam_id, "Family id");
          read_field(is, new_ind_id, "Indiv. id");
          read_field(is, new_fat_id, "Father id");
          read_field(is, new_mot_id, "Mother id");
          read_field(is, gender,     "Gender   ");
          read_field(is, pheno,      "Phenotype");
          std::vector<typename T_GENOTYPE::base> g= _gr.decode(is);
          // Add line to the pedigree
          multifamily_pedigree_t::pedigree_t* cped= NULL;
          if (new_fam_id == prev_family_id) {
            L_TRACE("New individual of the old family : " <<
                    new_fam_id);
            cped= &(ped.families().back());
          } else {
            L_TRACE("New family found: " << new_fam_id);
            cped= &(ped.add_family());
          }
          cped->set_length(g.size());
          prev_family_id= new_fam_id;
          // Individual
          L_TRACE("Adding individual: " << new_ind_id);
          multifamily_pedigree_t::pedigree_t::individual_t& ind=
            cped->get_by_id_or_create(new_ind_id);
          // Parents
          MY_ASSERT(missing_id_t<T_ID>::is_missing(new_fat_id) ==
                    missing_id_t<T_ID>::is_missing(new_mot_id));

          if (! missing_id_t<T_ID>::is_missing(new_fat_id)) {
            cped->get_by_id_or_create(new_fat_id);
            L_TRACE("Make sure that the father (" << new_fat_id << ") is present.");
            cped->get_by_id_or_create(new_mot_id);
            L_TRACE("Make sure that the mother (" << new_mot_id << ") is present.");
            cped->add_trio_by_id(new_ind_id, new_fat_id, new_mot_id);
          }
          // Gender and phenotype
          if (ind.gender().is_not_specified() ||
              gender.is_specified()) {
            ind.set_gender(gender);
          }
          ind.phenotype()= pheno;
          // Genotype
          std::copy(g.begin(), g.end(), ind.g().begin());
        } catch (invalid_line_t& e) {
          L_WARN("!! Discarded invalid line starting with >" <<
                 buff.substr(0, 20) << "<. Reason: " << e.msg);
        }
      }
    }
  }

};

#endif // __IN_PEDIGREE_HPP__
