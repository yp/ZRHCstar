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
 * pedigree.hpp
 *
 * Classes to represent genotyped and haplotyped pedigrees.
 *
 **/

#ifndef __PEDIGREE_HPP__
#define __PEDIGREE_HPP__


#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <climits>

#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "log.hpp"
#include "assertion.hpp"
#include "gender.hpp"
#include "haplotypes_genotypes.hpp"

template <typename T>
class not_existent_id_t {
};

template <typename T_ID>
class missing_id_t;

template <>
class missing_id_t<size_t> {
public:
  static bool is_missing(const size_t& _id) {
	 return _id == id();
  }
  static size_t id() {
	 return 0;
  }
};

template <typename T_GENOTYPE= genotype_t,
			 typename T_HAPLOTYPE= haplotype_t,
			 typename T_PHENOTYPE= std::string,
			 typename T_ID=size_t>
class basic_pedigree_t: boost::noncopyable {
public:
  friend class individual_t;
  friend class individuals_t;
  class individual_t;
  class individuals_t;

private:

  typedef boost::ptr_vector<individual_t> individuals_impl_t;

public:

  typedef T_GENOTYPE gen_t;
  typedef T_HAPLOTYPE hap_t;
  typedef T_PHENOTYPE phen_t;
  typedef T_ID id_t;
  typedef typename gen_t::base g;
  typedef typename hap_t::base h;

  class individuals_t:
	 public boost::noncopyable {
	 friend class basic_pedigree_t;
  private:
	 basic_pedigree_t<gen_t, hap_t, phen_t, id_t>* _p;
	 individuals_t(basic_pedigree_t<gen_t, hap_t, phen_t, id_t>* p)
		  : _p(p)
	 {}
  public:
	 typedef typename individuals_impl_t::iterator iterator;

	 iterator begin() {
		return _p->_indivs.begin();
	 };

	 iterator end() {
		return _p->_indivs.end();
	 };

	 typedef typename individuals_impl_t::reverse_iterator reverse_iterator;

	 reverse_iterator rbegin() {
		return _p->_indivs.rbegin();
	 };

	 reverse_iterator rend() {
		return _p->_indivs.rend();
	 };

	 typedef typename individuals_impl_t::const_iterator const_iterator;

	 const_iterator begin() const {
		return _p->_indivs.begin();
	 };

	 const_iterator end() const {
		return _p->_indivs.end();
	 };

	 typedef typename individuals_impl_t::const_reverse_iterator const_reverse_iterator;

	 const_reverse_iterator rbegin() const {
		return _p->_indivs.rbegin();
	 };

	 const_reverse_iterator rend() const {
		return _p->_indivs.rend();
	 };

	 size_t size() const {
		return _p->_indivs.size();
	 };

  };

  static const id_t not_existent_id;
  static const size_t not_existent_progr;

  class individual_t: boost::noncopyable {
	 friend class basic_pedigree_t;
  private:
	 basic_pedigree_t<gen_t, hap_t, phen_t, id_t>& _p;
	 const size_t _progr_id;
	 individual_t(basic_pedigree_t<gen_t, hap_t, phen_t, id_t>& p,
					  const size_t progr_id)
		  : _p(p), _progr_id(progr_id)
	 {}

  public:
	 const id_t& id() const {
		return _p._real_ids[_progr_id];
	 }
	 size_t progr_id() const {
		return _progr_id;
	 }
	 void set_id(const id_t& real_id) {
		TRACE("Changing id of individual " << _progr_id << " from " <<
				(id()) << " to " << real_id);
		MY_ASSERT((real_id == id()) ||
					 (real_id == not_existent_id) ||
					 (_p.get_progr_id(real_id) == not_existent_progr));
		if (id() != not_existent_id) {
		  MY_ASSERT(_p.get_progr_id(id()) == _progr_id);
		  _p._real2progr.erase(_p._real_ids[_progr_id]);
		}
		_p._real_ids[_progr_id]= real_id;
		if (real_id != not_existent_id) {
		  _p._real2progr[real_id]= _progr_id;
		}
	 }

	 const hap_t& hp() const {
		return _p._hp[_progr_id];
	 }
	 hap_t& hp() {
		return _p._hp[_progr_id];
	 }
	 const typename hap_t::base& hp(const size_t pos) const {
		return hp()[pos];
	 }
	 typename hap_t::base& hp(const size_t pos) {
		return hp()[pos];
	 }
	 const hap_t& hm() const {
		return _p._hm[_progr_id];
	 }
	 hap_t& hm() {
		return _p._hm[_progr_id];
	 }
	 const typename hap_t::base& hm(const size_t pos) const {
		return hm()[pos];
	 }
	 typename hap_t::base& hm(const size_t pos) {
		return hm()[pos];
	 }
	 const gen_t& g() const {
		return _p._g[_progr_id];
	 }
	 gen_t& g() {
		return _p._g[_progr_id];
	 }
	 const typename gen_t::base& g(const size_t pos) const {
		return g()[pos];
	 }
	 typename gen_t::base& g(const size_t pos) {
		return g()[pos];
	 }

	 const phen_t& phenotype() const {
		return _p._pheno[_progr_id];
	 }
	 phen_t& phenotype() {
		return _p._pheno[_progr_id];
	 }

	 bool has_father() const {
		return _p._fathers[_progr_id]!=not_existent_progr;
	 }
	 const individual_t& father() const {
		MY_ASSERT(has_father());
		return _p._indivs[_p._fathers[_progr_id]];
	 }
	 individual_t& father() {
		MY_ASSERT(has_father());
		return _p._indivs[_p._fathers[_progr_id]];
	 }
	 bool has_mother() const {
		return _p._mothers[_progr_id]!=not_existent_progr;
	 }
	 const individual_t& mother() const {
		MY_ASSERT(has_mother());
		return _p._indivs[_p._mothers[_progr_id]];
	 }
	 individual_t& mother() {
		MY_ASSERT(has_mother());
		return _p._indivs[_p._mothers[_progr_id]];
	 }
	 bool has_children() const {
		return !(_p._children[_progr_id].empty());
	 }

	 bool is_founder() const {
		return (!has_mother()) && (!has_father());
	 }

	 const gender_t& gender() const {
		return _p._genders[_progr_id];
	 }
	 void set_gender(const gender_t& new_gender) {
		MY_ASSERT((new_gender == gender()) ||
					 (!has_children()));
		_p._genders[_progr_id]= new_gender;
	 }



  };

private:

  size_t _len;

  individuals_impl_t _indivs;
  individuals_t _individuals;
  std::vector<id_t> _real_ids;
  std::vector<gender_t> _genders;
  std::map<id_t, size_t> _real2progr;

  boost::ptr_vector<haplotype_t> _hp;
  boost::ptr_vector<haplotype_t> _hm;
  boost::ptr_vector<genotype_t> _g;
  std::vector<phen_t> _pheno;

  std::vector<size_t> _fathers;
  std::vector<size_t> _mothers;
  std::vector< std::list<size_t> > _children;

protected:

  static log4cxx::LoggerPtr logger;

  size_t get_progr_id(const id_t& real_id) const {
	 typename std::map<id_t, size_t>::const_iterator it= _real2progr.find(real_id);
	 if (it == _real2progr.end()) {
		TRACE("Identifier " << real_id << " not found!");
		return not_existent_progr;
	 } else {
		return it->second;
	 }
  }

public:

  explicit basic_pedigree_t(const size_t len=0)
		:_len(len), _individuals(this)
  {
	 TRACE("Created an empty pedigree with genotype/haplotype length equal to " << _len);
  }

  ~basic_pedigree_t() {
	 TRACE("Destroying pedigree");
  }

  void set_length(const size_t len) {
	 MY_ASSERT((_len == 0) || (len == _len));
	 _len= len;
  }

  individual_t& add_individual(void) {
	 TRACE("Adding a new individual with progr_id = " << _indivs.size());
	 if (_len == 0) {
		FATAL("Genotype/haplotype length must be assigned before the first individual is added.");
		MY_FAIL;
	 }
	 _indivs.push_back(new individual_t(*this, _indivs.size()));
	 _real_ids.push_back(not_existent_id);
	 _genders.push_back(gender_t::UNSPEC);
	 _hp.push_back(new haplotype_t(_len));
	 _hm.push_back(new haplotype_t(_len));
	 _g.push_back(new genotype_t(_len));
	 _pheno.push_back(phen_t());
	 _fathers.push_back(not_existent_progr);
	 _mothers.push_back(not_existent_progr);
	 _children.push_back(std::list<size_t>());
	 return _indivs.back();
  }

  individual_t& add_individual(const id_t& real_id) {
	 MY_ASSERT( real_id != not_existent_id );
	 MY_ASSERT( ! exists(real_id) );
	 individual_t& ind= add_individual();
	 ind.set_id(real_id);
	 return ind;
  }

  individual_t& get_by_progr(const size_t progr_id) {
	 TRACE("Getting individual with progr_id = " << progr_id);
	 MY_ASSERT(progr_id < _indivs.size());
	 return _indivs[progr_id];
  }

  const individual_t& get_by_progr(const size_t progr_id) const {
	 TRACE("Getting individual with progr_id = " << progr_id);
	 MY_ASSERT(progr_id < _indivs.size());
	 return _indivs[progr_id];
  }

  individual_t& get_by_id(const id_t& real_id) {
	 TRACE("Getting individual with real_id = " << real_id);
	 MY_ASSERT(real_id != not_existent_id);
	 return get_by_progr(get_progr_id(real_id));
  }

  const individual_t& get_by_id(const id_t& real_id) const {
	 TRACE("Getting individual with real_id = " << real_id);
	 MY_ASSERT(real_id != not_existent_id);
	 return get_by_progr(get_progr_id(real_id));
  }

  bool exists(const id_t& real_id) {
	 return get_progr_id(real_id) != not_existent_progr;
  }

  individual_t& get_by_id_or_create(const id_t& real_id) {
	 TRACE("Searching for individual with real_id = " << real_id);
	 MY_ASSERT(real_id != not_existent_id);
	 size_t progr= get_progr_id(real_id);
	 if (progr == not_existent_progr) {
		TRACE("Individual not found. Creating...");
		individual_t& ind= add_individual();
		ind.set_id(real_id);
		return ind;
	 } else {
		TRACE("Individual found. Returning...");
		return get_by_progr(progr);
	 }
  }

  individual_t& operator[](const id_t& real_id) {
	 return get_by_id(real_id);
  }

  const individual_t& operator[](const id_t& real_id) const {
	 return get_by_id(real_id);
  }

  void add_trio_by_progr(const size_t progr_id,
								 const size_t progr_father_id,
								 const size_t progr_mother_id) {
	 TRACE("Adding trio by progressive id (" << progr_id << ", "
			 << "father: " << progr_father_id << ", "
			 << "mother: " << progr_mother_id << ").");
	 MY_ASSERT(progr_id < _indivs.size());
	 MY_ASSERT((progr_father_id < _indivs.size()) ||
				  (progr_father_id == not_existent_progr));
	 MY_ASSERT((progr_mother_id < _indivs.size()) ||
				  (progr_mother_id == not_existent_progr));
	 MY_ASSERT((_fathers[progr_id] == not_existent_progr) ||
				  (progr_father_id == not_existent_progr) ||
				  (_fathers[progr_id] == progr_father_id));
	 MY_ASSERT((_mothers[progr_id] == not_existent_progr) ||
				  (progr_mother_id == not_existent_progr) ||
				  (_mothers[progr_id] == progr_mother_id));
	 if (progr_father_id != not_existent_progr) {
		MY_ASSERT(_genders[progr_father_id].is_not_female());
		_genders[progr_father_id]= gender_t::MALE;
		_children[progr_father_id].push_back(progr_id);
	 }
	 if (progr_mother_id != not_existent_progr) {
		MY_ASSERT(_genders[progr_mother_id].is_not_male());
		_genders[progr_mother_id]= gender_t::FEMALE;
		_children[progr_mother_id].push_back(progr_id);
	 }
	 _fathers[progr_id]= progr_father_id;
	 _mothers[progr_id]= progr_mother_id;
  }

  void add_trio_by_id(const id_t& id,
							 const id_t& father_id,
							 const id_t& mother_id) {
	 TRACE("Adding trio by real id (" << id << ", "
			 << "father: " << father_id << ", "
			 << "mother: " << mother_id << ").");
	 const size_t progr_id= get_progr_id(id);
	 const size_t progr_father_id= get_progr_id(father_id);
	 const size_t progr_mother_id= get_progr_id(mother_id);
	 MY_ASSERT(progr_id != not_existent_progr);
	 MY_ASSERT((father_id == not_existent_id) ||
				  (progr_father_id != not_existent_progr));
	 MY_ASSERT((mother_id == not_existent_id) ||
				  (progr_mother_id != not_existent_progr));
	 add_trio_by_progr(progr_id, progr_father_id, progr_mother_id);
  }

  const individuals_t& individuals() const {
	 return _individuals;
  }

  individuals_t& individuals() {
	 return _individuals;
  }

  size_t size() const {
	 return _indivs.size();
  }

  size_t genotype_length() const {
	 return _len;
  }

};



template <>
class not_existent_id_t<size_t> {
public:
  static const size_t id= ULONG_MAX;
};

typedef basic_pedigree_t<> pedigree_t;




template <typename T_GENOTYPE, typename T_HAPLOTYPE,
			 typename T_PHENOTYPE, typename T_ID>
log4cxx::LoggerPtr basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE,T_PHENOTYPE,T_ID>::logger(log4cxx::Logger::getLogger("pedigree"));

template <typename T_GENOTYPE, typename T_HAPLOTYPE,
			 typename T_PHENOTYPE, typename T_ID>
const typename basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE,T_PHENOTYPE,T_ID>::id_t
basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE,T_PHENOTYPE,T_ID>::not_existent_id= not_existent_id_t<id_t>::id;

template <typename T_GENOTYPE, typename T_HAPLOTYPE,
			 typename T_PHENOTYPE, typename T_ID>
const size_t basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE,T_PHENOTYPE,T_ID>::not_existent_progr= ULONG_MAX;



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
class basic_multifamily_pedigree_t {
public:
  typedef basic_pedigree_t<T_GENOTYPE,
									T_HAPLOTYPE,
									T_PHENOTYPE,
									T_ID> pedigree_t;
  typedef T_VECTOR type;

private:
  const size_t _len;
  type _families;

public:
  basic_multifamily_pedigree_t(const size_t len= 0)
		:_len(len)
  {}

  pedigree_t& add_family() {
	 _families.push_back(new pedigree_t(_len));
	 return _families.back();
  }

  type& families() {
	 return _families;
  }

  const type& families() const {
	 return _families;
  }

  pedigree_t& operator[](size_t i) {
	 MY_ASSERT(i < _families.size());
	 return _families[i];
  }

  const pedigree_t& operator[](size_t i) const {
	 MY_ASSERT(i < _families.size());
	 return _families[i];
  }

  size_t size() const {
	 return _families.size();
  }

  size_t genotype_length() const {
	 return _len;
  }

};

typedef basic_multifamily_pedigree_t<> multifamily_pedigree_t;


#endif // __PEDIGREE_HPP__
