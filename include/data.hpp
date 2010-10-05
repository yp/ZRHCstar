/**
 *
 * data.hpp
 *
 * Classes to represent individuals, genotypes, and pedigrees.
 *
 **/

#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <algorithm>
#include <climits>
#include <boost/static_assert.hpp>

#include "log.hpp"
#include "assertion.hpp"
#include "utility.hpp"
#include "gender.hpp"

/**
 *
 *
 *
 * Representation of single-locus genotypes
 *
 *
 *
 **/

template <int T_HOMO1= 0, int T_HOMO2= 1, int T_HETER= 2, int T_MISS= 5>
class single_biallelic_genotype_t
  :public enum_like_t<single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>, 4, 3> {
private:

  BOOST_STATIC_ASSERT( (0 <= T_HOMO1) && (T_HOMO1 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_HOMO2) && (T_HOMO2 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_HETER) && (T_HETER < 10));
  BOOST_STATIC_ASSERT( (0 <= T_MISS) && (T_MISS < 10));
  BOOST_STATIC_ASSERT( (T_HOMO1 != T_HOMO2) && (T_HOMO1 != T_HETER) &&
							  (T_HOMO1 != T_MISS) && (T_HOMO2 != T_HETER) &&
							  (T_HOMO2 != T_MISS) && (T_HETER != T_MISS) );

  typedef enum_like_t<single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>, 4, 3> base;

  enum _values {
	 _HOMO1= T_HOMO1,
	 _HOMO2= T_HOMO2,
	 _HETER= T_HETER,
	 _MISS= T_MISS
  };

  single_biallelic_genotype_t(const int g)
		: base(g)
  {
	 MY_ASSERT( (g == _HOMO1) ||
				(g == _HOMO2) ||
				(g == _HETER) ||
				(g == _MISS)
				);
  }

public:

  single_biallelic_genotype_t()
		: base(_MISS)
  {}

  static const single_biallelic_genotype_t HOMO1;
  static const single_biallelic_genotype_t HOMO2;
  static const single_biallelic_genotype_t HETER;
  static const single_biallelic_genotype_t MISS;

  static const int values[];
  static const single_biallelic_genotype_t enum_values[];
};

typedef single_biallelic_genotype_t<> std_single_biallelic_genotype_t;


template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO1(T_HOMO1);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO2(T_HOMO2);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER(T_HETER);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::MISS(T_MISS);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const int single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::values[]= {T_HOMO1, T_HOMO2, T_HETER, T_MISS};

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::enum_values[]= {
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO1,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO2,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::MISS
};




/**
 *
 *
 *
 * Representation of single-locus haplotypes
 *
 *
 *
 **/


template <int T_ALLELE1= 0, int T_ALLELE2= 1, int T_MISS= 5>
class single_biallelic_haplotype_t
  :public enum_like_t<single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>, 3, 2> {
private:

  BOOST_STATIC_ASSERT( (0 <= T_ALLELE1) && (T_ALLELE1 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_ALLELE2) && (T_ALLELE2 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_MISS) && (T_MISS < 10));
  BOOST_STATIC_ASSERT( (T_ALLELE1 != T_ALLELE2) && (T_ALLELE1 != T_MISS) && (T_ALLELE2 != T_MISS) );

  typedef enum_like_t<single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>, 3, 2> base;

  enum _values {
	 _ALLELE1= T_ALLELE1,
	 _ALLELE2= T_ALLELE2,
	 _MISS= T_MISS
  };

  single_biallelic_haplotype_t(const int h)
		: base(h)
  {
	 MY_ASSERT( (h == _ALLELE1) ||
				(h == _ALLELE2) ||
				(h == _MISS)
				);
  }

public:

  single_biallelic_haplotype_t()
		: base(_MISS)
  {}

  static const single_biallelic_haplotype_t ALLELE1;
  static const single_biallelic_haplotype_t ALLELE2;
  static const single_biallelic_haplotype_t MISS;

  static const int values[];
  static const single_biallelic_haplotype_t enum_values[];

};



typedef single_biallelic_haplotype_t<> std_single_biallelic_haplotype_t;

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE1(T_ALLELE1);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE2(T_ALLELE2);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::MISS(T_MISS);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const int single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::values[]= {T_ALLELE1, T_ALLELE2, T_MISS};

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::enum_values[]= {
  single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE1,
  single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE2,
  single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::MISS
};


/**
 *
 *
 *
 * Representation of multi-locus genotypes and haplotype
 *
 *
 *
 **/

template <class base_t>
class default_base_writer_t {
private:
  std::ostream& _out;
public:
  default_base_writer_t(std::ostream& out)
		: _out(out)
  {}

  std::ostream& operator()(const base_t& val) {
	 return (_out << val);
  }
};

template <class base_t>
class default_base_reader_t {
private:
  std::istream& _in;
public:
  default_base_reader_t(std::istream& in)
		: _in(in)
  {}

  std::istream& operator()(base_t& val) {
	 return (_in >> val);
  }
};


template <class base_t,
			 class _reader=default_base_reader_t<base_t>,
			 class _writer=default_base_writer_t<base_t> >
class generic_fixlen_vector_t {
private:
  const size_t _len;
  base_t* _v;

public:

  typedef base_t base;
  typedef _reader reader;
  typedef _writer writer;


  generic_fixlen_vector_t(const size_t len)
		:_len(len), _v(new base_t[len])
  {}

  generic_fixlen_vector_t(const std::vector<base_t> v)
		:_len(v.size()), _v(new base_t[v.size()])
  {
	 std::copy(v.begin(), v.end(), _v);
  }

  ~generic_fixlen_vector_t() {
	 delete [] _v;
  }

  const base_t& operator[](const size_t pos) const {
	 MY_ASSERT(pos < _len);
	 return _v[pos];
  }

  base_t& operator[](const size_t pos) {
	 MY_ASSERT(pos < _len);
	 return _v[pos];
  }

  friend std::ostream& operator<<(std::ostream& out,
											 const generic_fixlen_vector_t& val) {
	 std::for_each(val._v, val._v+val._len, writer(out));
	 return out;
  }

  friend std::istream& operator>>(std::istream& in, generic_fixlen_vector_t& val) {
	 std::for_each(val._v, val._v+val._len, reader(in));
	 return in;
  }

};

typedef generic_fixlen_vector_t<std_single_biallelic_genotype_t> genotype_t;
typedef generic_fixlen_vector_t<std_single_biallelic_haplotype_t> haplotype_t;






/**
 *
 *
 *
 * Representation of genotyped and haplotyped pedigree
 *
 *
 *
 **/

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
		return (*(_p._hp[_progr_id]));
	 }
	 hap_t& hp() {
		return (*(_p._hp[_progr_id]));
	 }
	 const typename hap_t::base& hp(const size_t pos) const {
		return hp()[pos];
	 }
	 typename hap_t::base& hp(const size_t pos) {
		return hp()[pos];
	 }
	 const hap_t& hm() const {
		return (*(_p._hm[_progr_id]));
	 }
	 hap_t& hm() {
		return (*(_p._hm[_progr_id]));
	 }
	 const typename hap_t::base& hm(const size_t pos) const {
		return hm()[pos];
	 }
	 typename hap_t::base& hm(const size_t pos) {
		return hm()[pos];
	 }
	 const gen_t& g() const {
		return (*(_p._g[_progr_id]));
	 }
	 gen_t& g() {
		return (*(_p._g[_progr_id]));
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

  std::vector<individual_t*> _indivs;
  std::vector<size_t> _real_ids;
  std::vector<gender_t> _genders;
  std::map<size_t, size_t> _real2progr;

  std::vector<haplotype_t*> _hp;
  std::vector<haplotype_t*> _hm;
  std::vector<genotype_t*> _g;

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
		return (*it).second;
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
	 deleter_t<individual_t> d_ind;
	 deleter_t<haplotype_t> d_hap;
	 deleter_t<genotype_t> d_gen;
	 std::for_each(_indivs.begin(), _indivs.end(), d_ind);
	 std::for_each(_hp.begin(), _hp.end(), d_hap);
	 std::for_each(_hm.begin(), _hm.end(), d_hap);
	 std::for_each(_g.begin(), _g.end(), d_gen);
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
	 return *_indivs.back();
  }

  individual_t& get_by_progr(const size_t progr_id) {
	 TRACE("Getting individual with progr_id = " << progr_id);
	 MY_ASSERT(progr_id < _indivs.size());
	 return *(_indivs[progr_id]);
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
	 MY_ASSERT(progr_father_id < _indivs.size());
	 MY_ASSERT(progr_mother_id < _indivs.size());
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


#endif // __DATA_HPP__
