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
 * haplotypes_genotypes.hpp
 *
 * Classes to represent haplotypes and genotypes.
 *
 **/

#ifndef __HAPLOTYPES_GENOTYPES_HPP__
#define __HAPLOTYPES_GENOTYPES_HPP__

#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <boost/static_assert.hpp>

#include "log.hpp"
#include "assertion.hpp"
#include "utility.hpp"

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
	 _HOMO1,
	 _HOMO2,
	 _HETER,
	 _MISS
  };

  single_biallelic_genotype_t(const int g)
		: base(g)
  {
  }

public:

  single_biallelic_genotype_t()
		: base(_MISS)
  {}

  static const single_biallelic_genotype_t HOMO1;
  static const single_biallelic_genotype_t HOMO2;
  static const single_biallelic_genotype_t HETER;
  static const single_biallelic_genotype_t MISS;

  static const int int_values[];
  static const std::string str_values[];
  static const single_biallelic_genotype_t enum_values[];
};


typedef single_biallelic_genotype_t<> std_single_biallelic_genotype_t;

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO1(_HOMO1);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO2(_HOMO2);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER(_HETER);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::MISS(_MISS);

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const int single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::int_values[]= {T_HOMO1, T_HOMO2, T_HETER, T_MISS};

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const std::string single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::str_values[]= {
  std::string(1, '0'+T_HOMO1),
  std::string(1, '0'+T_HOMO2),
  std::string(1, '0'+T_HETER),
  std::string(1, '0'+T_MISS)
};

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>
single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::enum_values[]= {
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO1,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HOMO2,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER,
  single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::MISS
};


template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
bool is_genotyped(const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>& g) {
  return g != single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::MISS;
}

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
bool is_homozigous(const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>& g) {
  return
	 is_genotyped(g) &&
	 (g != single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER);
}

template <int T_HOMO1, int T_HOMO2, int T_HETER, int T_MISS>
bool is_heterozygous(const single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>& g) {
  return
	 is_genotyped(g) &&
	 (g == single_biallelic_genotype_t<T_HOMO1, T_HOMO2, T_HETER, T_MISS>::HETER);
}

/**
 *
 *
 *
 * Representation of single-locus haplotypes
 *
 *
 *
 **/


template <int T_ALLELE1= 1, int T_ALLELE2= 2, int T_MISS= 0>
class single_biallelic_haplotype_t
  :public enum_like_t<single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>, 3, 2> {
private:

  BOOST_STATIC_ASSERT( (0 <= T_ALLELE1) && (T_ALLELE1 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_ALLELE2) && (T_ALLELE2 < 10));
  BOOST_STATIC_ASSERT( (0 <= T_MISS) && (T_MISS < 10));
  BOOST_STATIC_ASSERT( (T_ALLELE1 != T_ALLELE2) && (T_ALLELE1 != T_MISS) && (T_ALLELE2 != T_MISS) );

  typedef enum_like_t<single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>, 3, 2> base;

  enum _values {
	 _ALLELE1,
	 _ALLELE2,
	 _MISS
  };

  single_biallelic_haplotype_t(const int h)
		: base(h)
  {
  }

public:

  single_biallelic_haplotype_t()
		: base(_MISS)
  {}

  static const single_biallelic_haplotype_t ALLELE1;
  static const single_biallelic_haplotype_t ALLELE2;
  static const single_biallelic_haplotype_t MISS;

  static const int int_values[];
  static const std::string str_values[];
  static const single_biallelic_haplotype_t enum_values[];

};

template <typename h_t, typename g_t>
const h_t& homozygous_to_haplotype(const g_t& g) {
  MY_ASSERT_DBG( (g == g_t::HOMO1) || (g == g_t::HOMO2) );
  if        ( g == g_t::HOMO1 ) {
	 return h_t::ALLELE1;
  } else if ( g == g_t::HOMO2 ) {
	 return h_t::ALLELE2;
  } else {
	 MY_FAIL;
	 return h_t::MISS;
  }
};


template <typename h_t, typename g_t>
bool
haplotype_genotype_consistent(const h_t& h1, const h_t& h2,
										const g_t& g) {
  if ((h1 == h_t::MISS) && (h2 == h_t::MISS))
	 return true;
  if (! is_genotyped(g))
	 return true;
  if ((h1 == h_t::MISS) && (h2 != h_t::MISS))
	 return haplotype_genotype_consistent(h2, h1, g);

  MY_ASSERT( h1 != h_t::MISS );
  MY_ASSERT( is_genotyped(g) );

  if (is_homozigous(g)) {
	 return
		((h2 == h_t::MISS) || (h1 == h2)) &&
		(h1 == homozygous_to_haplotype<h_t, g_t>(g));
  } else {
	 return (h2 == h_t::MISS) || (h1 != h2);
  }
};

template <typename h_t, typename g_t>
bool
strict_haplotype_genotype_consistent(const h_t& h1, const h_t& h2,
												 const g_t& g) {
  if ((h1 == h_t::MISS) || (h2 == h_t::MISS))
	 return false;
  else
	 return haplotype_genotype_consistent(h1, h2, g);
}

typedef single_biallelic_haplotype_t<> std_single_biallelic_haplotype_t;

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE1(_ALLELE1);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::ALLELE2(_ALLELE2);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>
single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::MISS(_MISS);

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const int single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::int_values[]= {
  T_ALLELE1,
  T_ALLELE2,
  T_MISS
};

template <int T_ALLELE1, int T_ALLELE2, int T_MISS>
const std::string single_biallelic_haplotype_t<T_ALLELE1, T_ALLELE2, T_MISS>::str_values[]= {
  std::string(1, '0'+T_ALLELE1),
  std::string(1, '0'+T_ALLELE2),
  std::string(1, '0'+T_MISS),
};

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


template <class _base_t,
			 class _reader=default_base_reader_t<_base_t>,
			 class _writer=default_base_writer_t<_base_t> >
class generic_fixlen_vector_t {
public:

  typedef _base_t base;
  typedef _reader reader;
  typedef _writer writer;

private:
  const size_t _len;
  base* _v;

public:

  generic_fixlen_vector_t(const size_t len)
		:_len(len), _v(new base[len])
  {}

  generic_fixlen_vector_t(const std::vector<base> v)
		:_len(v.size()), _v(new base[v.size()])
  {
	 std::copy(v.begin(), v.end(), _v);
  }

  ~generic_fixlen_vector_t() {
	 delete [] _v;
  }

  const base& operator[](const size_t pos) const {
	 MY_ASSERT_DBG(pos < _len);
	 return _v[pos];
  }

  base& operator[](const size_t pos) {
	 MY_ASSERT_DBG(pos < _len);
	 return _v[pos];
  }

  base const * begin() const {
	 return _v;
  }

  base* begin() {
	 return _v;
  }

  base const * end() const {
	 return _v+_len;
  }

  base * end() {
	 return _v+_len;
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

  size_t size() const {
	 return _len;
  };

  bool is_compatible_with(const generic_fixlen_vector_t<_base_t,_reader,_writer>& v) const {
  typedef generic_fixlen_vector_t<_base_t,_reader,_writer> v_t;
  if (size() != v.size())
	 return false;
  const typename v_t::base* v1it= begin();
  const typename v_t::base* v2it= v.begin();
  for (; v1it != end(); ++v1it, ++v2it) {
	 if ( (*v1it != v_t::base::MISS) &&
			(*v2it != v_t::base::MISS) &&
			(*v1it != *v2it) ) {
		return false;
	 }
  }
  return true;
};

};

template <typename h_t, typename g_t>
bool
multilocus_haplotype_genotype_consistent(const h_t& h1,
													  const h_t& h2,
													  const g_t& g) {
  MY_ASSERT_DBG(g.size() == h1.size());
  MY_ASSERT_DBG(g.size() == h2.size());
  const typename g_t::base* git= g.begin();
  const typename h_t::base* h1it= h1.begin();
  const typename h_t::base* h2it= h2.begin();
  for (; git != g.end(); ++git, ++h1it, ++h2it) {
	 if (! haplotype_genotype_consistent(*h1it, *h2it, *git)) {
		return false;
	 }
  }
  return true;
}

template <typename h_t, typename g_t>
bool
strict_multilocus_haplotype_genotype_consistent(const h_t& h1,
																const h_t& h2,
																const g_t& g) {
  MY_ASSERT_DBG(g.size() == h1.size());
  MY_ASSERT_DBG(g.size() == h2.size());
  const typename g_t::base* git= g.begin();
  const typename h_t::base* h1it= h1.begin();
  const typename h_t::base* h2it= h2.begin();
  for (; git != g.end(); ++git, ++h1it, ++h2it) {
	 if (! strict_haplotype_genotype_consistent(*h1it, *h2it, *git)) {
		return false;
	 }
  }
  return true;
}

typedef generic_fixlen_vector_t<std_single_biallelic_genotype_t> genotype_t;
typedef generic_fixlen_vector_t<std_single_biallelic_haplotype_t> haplotype_t;

template <class _base_t,
			 class _reader,
			 class _writer>
bool operator==(const generic_fixlen_vector_t<_base_t,_reader,_writer>& v1,
					 const generic_fixlen_vector_t<_base_t,_reader,_writer>& v2) {
  typedef generic_fixlen_vector_t<_base_t,_reader,_writer> v_t;
  if (v1.size() != v2.size())
	 return false;
  const typename v_t::base* v1it= v1.begin();
  const typename v_t::base* v2it= v2.begin();
  for (; v1it != v1.end(); ++v1it, ++v2it) {
	 if ( (*v1it) != (*v2it) ) {
		return false;
	 }
  }
  return true;
};

template <class _base_t,
			 class _reader,
			 class _writer>
bool operator!=(const generic_fixlen_vector_t<_base_t,_reader,_writer>& v1,
					 const generic_fixlen_vector_t<_base_t,_reader,_writer>& v2) {
  typedef generic_fixlen_vector_t<_base_t,_reader,_writer> v_t;
  if (v1.size() != v2.size())
	 return true;
  const typename v_t::base* v1it= v1.begin();
  const typename v_t::base* v2it= v2.begin();
  for (; v1it != v1.end(); ++v1it, ++v2it) {
	 if ( (*v1it) != (*v2it) ) {
		return true;
	 }
  }
  return false;
};

#endif // __HAPLOTYPES_GENOTYPES_HPP__
