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
 * io-haplotypes_genotypes.hpp
 *
 * Classes to represent readers and writers of genotypes and haplotypes.
 *
 **/

#ifndef __IO_HAPLOTYPES_GENOTYPES_HPP__
#define __IO_HAPLOTYPES_GENOTYPES_HPP__

#include "haplotypes_genotypes.hpp"
#include "log.hpp"

#include <sstream>
#include <vector>
#include <string>

template <typename T_BASE_GENOTYPE>
class basic_genotype_reader_t
:public log_able_t< basic_genotype_reader_t< T_BASE_GENOTYPE > >
{
protected:

  virtual bool decode_next(std::istream& genotype_stream,
                           std::vector<T_BASE_GENOTYPE>& v) = 0;

public:
  std::vector<T_BASE_GENOTYPE> decode(const std::string& genotype) {
    std::istringstream is(genotype);
    return decode(is);
  }

  std::vector<T_BASE_GENOTYPE> decode(std::istream& genotype_stream) {
    std::vector<T_BASE_GENOTYPE> v;
    while (decode_next(genotype_stream, v)) {
// do nothing
    }
    return v;
  }
};

template <
  int T_ALLELE1= 1, int T_ALLELE2= 2, int T_ALLELEMISS= 0,
  int T_HOMO1= 0, int T_HOMO2= 1, int T_HETER= 2, int T_MISS= 5>
class biallelic_genotype_reader_t
  :public basic_genotype_reader_t< single_biallelic_genotype_t< T_HOMO1,
                                                                T_HOMO2,
                                                                T_HETER,
                                                                T_MISS>
                                   >
{
protected:
  typedef single_biallelic_genotype_t< T_HOMO1,
                                       T_HOMO2,
                                       T_HETER,
                                       T_MISS> g_t;

  virtual bool
  decode_next(std::istream& genotype_stream,
              std::vector<g_t>& v) {
    bool ris= true;
    int a1, a2;
    genotype_stream >> a1 >> a2;
    if (genotype_stream) {
      L_TRACE("Read alleles " << a1 << " and " << a2 << ".");
      MY_ASSERT((a1 == T_ALLELE1) || (a1 == T_ALLELE2) || (a1 == T_ALLELEMISS));
      MY_ASSERT((a2 == T_ALLELE1) || (a2 == T_ALLELE2) || (a2 == T_ALLELEMISS));
      if (a1 == a2) {
        if (a1 == T_ALLELE1) {
          v.push_back(g_t::HOMO1);
        } else if (a2 == T_ALLELE2) {
          v.push_back(g_t::HOMO2);
        } else {
          v.push_back(g_t::MISS);
        }
      } else {
        MY_ASSERT((a1 != T_ALLELEMISS) && (a2 != T_ALLELEMISS));
        v.push_back(g_t::HETER);
      }
    } else {
      if (!genotype_stream.eof()) {
        L_WARN("Error while decoding the genotype.");
      }
      ris= false;
    }
    return ris;
  }
};




template <typename T_BASE>
class basic_vector_writer_t
:public log_able_t< basic_vector_writer_t< T_BASE > >
{
protected:

  virtual void encode_next(std::ostream& vector_stream,
                           const T_BASE& v,
                           const std::string& sep= " ") const = 0;

public:
  template <typename T>
  std::string encode(const T& begin,
                     const T& end,
                     const std::string& sep= " ") const {
    std::ostringstream os;
    encode(os, begin, end, sep);
    return os.str();
  }

  template <typename T>
  void encode(std::ostream& vector_stream,
              const T& begin,
              const T& end,
              const std::string& sep= " ") const {
    for(T it= begin; it != end; ++it) {
      if (it != begin) {
        vector_stream << sep;
      }
      encode_next(vector_stream, *it, sep);
    }
  }

  template <typename T>
  std::string encode(const T& vector,
                     const std::string& sep= " ") const {
    return encode(vector.begin(), vector.end(), sep);
  }

  template <typename T>
  void encode(std::ostream& vector_stream,
              const T& vector,
              const std::string& sep= " ") const {
    encode(vector_stream, vector.begin(), vector.end(), sep);
  }
};


template <
  int T_ALLELE1= 1, int T_ALLELE2= 2, int T_ALLELEMISS= 0,
  int T_HOMO1= 0, int T_HOMO2= 1, int T_HETER= 2, int T_MISS= 5>
class biallelic_genotype_writer_t
  :public basic_vector_writer_t< single_biallelic_genotype_t< T_HOMO1,
																				  T_HOMO2,
																				  T_HETER,
																				  T_MISS>
                                   >
{
protected:
  typedef single_biallelic_genotype_t< T_HOMO1,
                                       T_HOMO2,
                                       T_HETER,
                                       T_MISS> g_t;

  virtual void encode_next(std::ostream& genotype_stream,
                           const g_t& g,
                           const std::string& sep= " ") const {
    if ( g == g_t::HOMO1 ) {
      genotype_stream << T_ALLELE1 << sep << T_ALLELE1;
    } else if ( g == g_t::HOMO2 ) {
      genotype_stream << T_ALLELE2 << sep << T_ALLELE2;
    } else if ( g == g_t::HETER ) {
      genotype_stream << T_ALLELE1 << sep << T_ALLELE2;
    } else if ( g == g_t::MISS  ) {
      genotype_stream << T_ALLELEMISS << sep << T_ALLELEMISS;
    } else {
      MY_FAIL;
    }
  };

};




template <
  int T_ALLELE1= 1, int T_ALLELE2= 2, int T_ALLELEMISS= 0
>
class biallelic_haplotype_writer_t
  :public basic_vector_writer_t< single_biallelic_haplotype_t< T_ALLELE1,
																					T_ALLELE2,
																					T_ALLELEMISS >
											>
{
protected:
  typedef single_biallelic_haplotype_t< T_ALLELE1,
													 T_ALLELE2,
													 T_ALLELEMISS > h_t;

  virtual void encode_next(std::ostream& haplotype_stream,
									const h_t& h,
									const std::string& sep= " ") const {
	 if        ( h == h_t::ALLELE1 ) {
		haplotype_stream << T_ALLELE1;
	 } else if ( h == h_t::ALLELE2 ) {
		haplotype_stream << T_ALLELE2;
	 } else if ( h == h_t::MISS ) {
		haplotype_stream << T_ALLELEMISS;
	 } else {
		MY_FAIL;
	 }
  };

};



template <typename T_BASE>
class basic_double_vector_writer_t
:public basic_vector_writer_t< T_BASE >
{
protected:

  virtual void encode_next(std::ostream& vector_stream,
									const T_BASE& v1,
									const T_BASE& v2,
									const std::string& insep= "|") const = 0;

public:
  template <typename T>
  std::string encode(const T& begin1,
							const T& end1,
							const T& begin2,
							const T& end2,
							const std::string& outsep= " ",
							const std::string& insep= "|") const {
	 std::ostringstream os;
	 encode(os, begin1, end1, begin2, end2, outsep, insep);
	 return os.str();
  }

  template <typename T>
  void encode(std::ostream& vector_stream,
				  const T& begin1,
				  const T& end1,
				  const T& begin2,
				  const T& end2,
				  const std::string& outsep= " ",
				  const std::string& insep= "|") const {
	 T it1= begin1;
	 T it2= begin2;
	 for(; it1 != end1; ++it1, ++it2) {
		MY_ASSERT( it2 != end2 );
		if (it1 != begin1) {
		  MY_ASSERT( it2 != begin2 );
		  vector_stream << outsep;
		}
		encode_next(vector_stream, *it1, *it2, insep);
	 }
	 MY_ASSERT( (it1 == end1) && (it2 == end2) );
  }

  template <typename T>
  std::string encode(const T& vector1,
							const T& vector2,
							const std::string& outsep= " ",
							const std::string& insep= "|") const {
	 return encode(vector1.begin(), vector1.end(),
						vector2.begin(), vector2.end(),
						outsep, insep);
  }

  template <typename T>
  void encode(std::ostream& vector_stream,
				  const T& vector1,
				  const T& vector2,
				  const std::string& outsep= " ",
				  const std::string& insep= "|") const {
	 encode(vector_stream,
			  vector1.begin(), vector1.end(),
			  vector2.begin(), vector2.end(),
			  outsep, insep);
  }
};

template <
  int T_ALLELE1= 1, int T_ALLELE2= 2, int T_ALLELEMISS= 0
>
class biallelic_haplotype_pair_writer_t
  :public basic_double_vector_writer_t< single_biallelic_haplotype_t< T_ALLELE1,
																							 T_ALLELE2,
																							 T_ALLELEMISS >
													 >
{
protected:
  typedef single_biallelic_haplotype_t< T_ALLELE1,
													 T_ALLELE2,
													 T_ALLELEMISS > h_t;

  virtual void encode_next(std::ostream& haplotype_stream,
									const h_t& h,
									const std::string& sep= " ") const {
	 if        ( h == h_t::ALLELE1 ) {
		haplotype_stream << T_ALLELE1;
	 } else if ( h == h_t::ALLELE2 ) {
		haplotype_stream << T_ALLELE2;
	 } else if ( h == h_t::MISS ) {
		haplotype_stream << T_ALLELEMISS;
	 } else {
		MY_FAIL;
	 }
  };

  virtual void encode_next(std::ostream& haplotype_stream,
									const h_t& h1,
									const h_t& h2,
									const std::string& insep= "|") const {
	 encode_next(haplotype_stream, h1, "");
	 haplotype_stream << insep;
	 encode_next(haplotype_stream, h2, "");
  };

};

#endif // __IO_HAPLOTYPES_GENOTYPES_HPP__
