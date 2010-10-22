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
 * out-pedigree.hpp
 *
 * Classes to represent writers of genotyped and haplotyped
 * pedigrees.
 *
 **/

#ifndef __OUT_PEDIGREE_HPP__
#define __OUT_PEDIGREE_HPP__

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
class basic_out_multifamily_pedigree_t
  :public descriptive_class,
   public log_able_t< basic_out_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                                       T_PHENOTYPE, T_ID, T_VECTOR> >

{
public:
  typedef basic_multifamily_pedigree_t<T_GENOTYPE,
                                       T_HAPLOTYPE,
                                       T_PHENOTYPE,
                                       T_ID,
                                       T_VECTOR> multifamily_pedigree_t;

protected:

  virtual void write_impl(std::ostream& out,
                          const multifamily_pedigree_t& ped) const = 0;

public:

  void write(std::ostream& out, const multifamily_pedigree_t& ped) const {
    L_DEBUG("Printing multifamily pedigree with " << get_brief_description() << " writer...");
    write_impl(out, ped);
  };

};

#include <stack>

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
class plink_writer_t
  :public basic_out_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                           T_PHENOTYPE, T_ID, T_VECTOR>
{

private:

  basic_genotype_writer_t<typename T_GENOTYPE::base>& _gw;

  const std::string _sep;

  typedef basic_out_multifamily_pedigree_t<T_GENOTYPE, T_HAPLOTYPE,
                                           T_PHENOTYPE, T_ID, T_VECTOR> base_t;

public:
  plink_writer_t(
    basic_genotype_writer_t<typename T_GENOTYPE::base>& gw,
    const std::string& sep="\t"
    )
    :_gw(gw), _sep(sep)
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

  void write_single_family(std::ostream& out,
                           const std::string& id_fam,
                           const typename base_t::multifamily_pedigree_t::pedigree_t& ped) const {
    // Perform a topological ordering of individuals
    std::vector<bool> visited(ped.size(), false);
    std::vector<size_t> order;
    std::stack<size_t> stack;
    for (typename base_t::multifamily_pedigree_t::pedigree_t::individuals_t::const_iterator it= ped.individuals().begin();
         it != ped.individuals().end();
         ++it) {
      if (!visited[it->progr_id()]) {
        stack.push(it->progr_id());
      }
      while (!stack.empty()) {
        size_t cid= stack.top();
        stack.pop();
        MY_ASSERT(!visited[cid]);
        const typename base_t::multifamily_pedigree_t::pedigree_t::individual_t& ind= ped.get_by_progr(cid);
        if (ind.has_mother() &&
            !visited[ind.mother().progr_id()]) {
          stack.push(cid);
          stack.push(ind.mother().progr_id());
        } else if (ind.has_father() &&
                   !visited[ind.father().progr_id()]) {
          stack.push(cid);
          stack.push(ind.father().progr_id());
        } else {
          L_TRACE("Adding " << cid << " to the queue.");
          visited[cid]= true;
          order.push_back(cid);
        }
      }
    }
    for (std::vector<size_t>::const_iterator it= order.begin();
         it != order.end();
         ++it) {
        const typename base_t::multifamily_pedigree_t::pedigree_t::individual_t& ind= ped.get_by_progr(*it);
        out << id_fam << _sep;
        out << ind.id() << _sep;
        if (ind.has_father()) {
          out << ind.father().id() << _sep;
        } else {
          out << missing_id_t<T_ID>::id() << _sep;
        }
        if (ind.has_mother()) {
          out << ind.mother().id() << _sep;
        } else {
          out << missing_id_t<T_ID>::id() << _sep;
        }
        out << ind.gender() << _sep;
        out << ind.phenotype() << _sep;
        _gw.encode(out, ind.g(), _sep);
        out << std::endl;
    }

  }

  virtual void write_impl(std::ostream& out,
                          const multifamily_pedigree_t& ped) const {
    size_t i= 0;
    for (multifamily_pedigree_t::type::const_iterator it= ped.families().begin();
         it != ped.families().end();
         ++it) {
      std::ostringstream os;
      os << i;
      write_single_family(out, os.str(), *it);
      ++i;
    }
  }

};

#endif // __OUT_PEDIGREE_HPP__
