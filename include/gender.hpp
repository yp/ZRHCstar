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
 * gender.hpp
 *
 * Simple enum-style class to represent genders.
 *
 **/
#ifndef __GENDER_HPP__
#define __GENDER_HPP__

#include <iostream>

class gender_t {

private:
  enum gender_value {
    _UNSPEC= 0,
    _MALE= 1,
    _FEMALE= 2
  } _g;

  explicit gender_t(gender_value g)
    :_g(g)
    {}

public:
  static const gender_t UNSPEC, MALE, FEMALE;

  gender_t(const gender_t& base)
    :_g(base._g)
    {}

  gender_t()
    :_g(_UNSPEC)
    {};

  bool is_male() const {
    return _g == _MALE;
  }

  bool is_female() const {
    return _g == _FEMALE;
  }

  bool is_not_specified() const {
    return _g == _UNSPEC;
  }

  bool is_not_male() const {
    return _g != _MALE;
  }

  bool is_not_female() const {
    return _g != _FEMALE;
  }

  bool is_specified() const {
    return _g != _UNSPEC;
  }

  friend bool operator==(const gender_t& g1, const gender_t& g2) {
    return g1._g==g2._g;
  }

  friend bool operator!=(const gender_t& g1, const gender_t& g2) {
    return g1._g!=g2._g;
  }

  friend std::ostream& operator<<(std::ostream& out, const gender_t& val) {
    return (out << val._g);
  }

  template <int C_MALE= 1,
            int C_FEMALE= 2,
            int C_UNSPEC= 0>
  friend std::istream& operator>>(std::istream& in, gender_t& gender) {
    int val;
    if (in >> val) {
      if (val == C_MALE) {
        gender= MALE;
      } else if (val == C_FEMALE) {
        gender= FEMALE;
      } else if (val == C_UNSPEC) {
        gender= UNSPEC;
      } else {
        gender= UNSPEC;
        in.setstate(std::ios::failbit);
      }
    } else {
      gender= UNSPEC;
    }
    return in;
  }

};


#endif // __GENDER_HPP__
