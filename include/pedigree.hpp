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

#include <boost/ptr_container/ptr_vector.hpp>

#include "log.hpp"
#include "assertion.hpp"
#include "gender.hpp"
#include "haplotypes_genotypes.hpp"



template <typename T_GENOTYPE= genotype_t,
			 class T_HAPLOTYPE= haplotype_t>
class basic_pedigree_t {
  friend class individual_t;
public:

  typedef T_GENOTYPE gen_t;
  typedef T_HAPLOTYPE hap_t;
  typedef typename gen_t::base g;
  typedef typename hap_t::base h;

  static const size_t not_existent_id= ULONG_MAX;

  class individual_t {
	 friend class basic_pedigree_t;
  private:
	 basic_pedigree_t<gen_t, hap_t>& _p;
	 const size_t _progr_id;
	 individual_t(basic_pedigree_t<gen_t, hap_t>& p,
					  const size_t progr_id)
		  : _p(p), _progr_id(progr_id)
	 {}

  public:
	 size_t id() const {
		return _p._real_ids[_progr_id];
	 }
	 size_t progr_id() const {
		return _progr_id;
	 }
	 void set_id(const size_t& real_id) {
		TRACE("Changing id of individual " << _progr_id << " from " <<
				(id()) << " to " << real_id);
		MY_ASSERT((real_id == id()) ||
					 (real_id == not_existent_id) ||
					 (_p.get_progr_id(real_id) == not_existent_id));
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

	 bool has_father() const {
		return _p._fathers[_progr_id]!=not_existent_id;
	 }
	 bool has_mother() const {
		return _p._mothers[_progr_id]!=not_existent_id;
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

  const size_t _len;

  boost::ptr_vector<individual_t> _indivs;
  std::vector<size_t> _real_ids;
  std::vector<gender_t> _genders;
  std::map<size_t, size_t> _real2progr;

  boost::ptr_vector<haplotype_t> _hp;
  boost::ptr_vector<haplotype_t> _hm;
  boost::ptr_vector<genotype_t> _g;

  std::vector<size_t> _fathers;
  std::vector<size_t> _mothers;
  std::vector< std::list<size_t> > _children;

protected:

  static log4cxx::LoggerPtr logger;

  size_t get_progr_id(const int& real_id) const {
	 std::map<size_t, size_t>::const_iterator it= _real2progr.find(real_id);
	 if (it == _real2progr.end()) {
		DEBUG("Identifier " << real_id << " not found!");
		return not_existent_id;
	 } else {
		return it->second;
	 }
  }

public:

  basic_pedigree_t(const size_t len)
		:_len(len)
  {
	 TRACE("Created an empty pedigree with genotype/haplotype length equal to " << _len);
  }

  ~basic_pedigree_t() {
	 TRACE("Destroying pedigree");
  }

  individual_t& add_individual(void) {
	 TRACE("Adding a new individual with progr_id = " << _indivs.size());
	 _indivs.push_back(new individual_t(*this, _indivs.size()));
	 _real_ids.push_back(not_existent_id);
	 _genders.push_back(gender_t::UNSPEC);
	 _hp.push_back(new haplotype_t(_len));
	 _hm.push_back(new haplotype_t(_len));
	 _g.push_back(new genotype_t(_len));
	 _fathers.push_back(not_existent_id);
	 _mothers.push_back(not_existent_id);
	 _children.push_back(std::list<size_t>());
	 return _indivs.back();
  }

  individual_t& get_by_progr(const size_t progr_id) {
	 TRACE("Getting individual with progr_id = " << progr_id);
	 MY_ASSERT(progr_id < _indivs.size());
	 return _indivs[progr_id];
  }

  individual_t& get_by_id(const size_t real_id) {
	 TRACE("Getting individual with real_id = " << real_id);
	 return get_by_progr(get_progr_id(real_id));
  }

  individual_t& operator[](const size_t real_id) {
	 return get_by_id(real_id);
  }

  void add_trio_by_progr(const size_t progr_id,
								 const size_t progr_father_id,
								 const size_t progr_mother_id) {
	 TRACE("Adding trio (" << progr_id << ", "
			 << "father: " << progr_father_id << ", "
			 << "mother: " << progr_mother_id << ").");
	 MY_ASSERT(progr_id < _indivs.size());
	 MY_ASSERT((progr_father_id < _indivs.size()) ||
				  (progr_father_id == not_existent_id));
	 MY_ASSERT((progr_mother_id < _indivs.size()) ||
				  (progr_mother_id == not_existent_id));
	 MY_ASSERT((_fathers[progr_id] == not_existent_id) ||
				  (progr_father_id == not_existent_id) ||
				  (_fathers[progr_id] == progr_father_id));
	 MY_ASSERT((_mothers[progr_id] == not_existent_id) ||
				  (progr_mother_id == not_existent_id) ||
				  (_mothers[progr_id] == progr_mother_id));
	 if (progr_father_id != not_existent_id) {
		MY_ASSERT(_genders[progr_father_id].is_not_female());
		_genders[progr_father_id]= gender_t::MALE;
		_children[progr_father_id].push_back(progr_id);
	 }
	 if (progr_mother_id != not_existent_id) {
		MY_ASSERT(_genders[progr_mother_id].is_not_male());
		_genders[progr_mother_id]= gender_t::FEMALE;
		_children[progr_mother_id].push_back(progr_id);
	 }
	 _fathers[progr_id]= progr_father_id;
	 _mothers[progr_id]= progr_mother_id;
  }

  void add_trio_by_id(const size_t id,
							 const size_t father_id,
							 const size_t mother_id) {
	 TRACE("Adding trio (" << id << ", "
			 << "father: " << father_id << ", "
			 << "mother: " << mother_id << ").");
	 const size_t progr_id= get_progr_id(id);
	 const size_t progr_father_id= get_progr_id(father_id);
	 const size_t progr_mother_id= get_progr_id(mother_id);
	 MY_ASSERT(progr_id != not_existent_id);
	 MY_ASSERT((father_id == not_existent_id) ||
				  (progr_father_id != not_existent_id));
	 MY_ASSERT((mother_id == not_existent_id) ||
				  (progr_mother_id != not_existent_id));
	 add_trio_by_progr(progr_id, progr_father_id, progr_mother_id);
  }

};

typedef basic_pedigree_t<> pedigree_t;




template <typename T_GENOTYPE, typename T_HAPLOTYPE>
log4cxx::LoggerPtr basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE>::logger(log4cxx::Logger::getLogger("pedigree"));

template <typename T_GENOTYPE, typename T_HAPLOTYPE>
const size_t basic_pedigree_t<T_GENOTYPE,T_HAPLOTYPE>::not_existent_id;


#endif // __PEDIGREE_HPP__
