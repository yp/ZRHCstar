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
 * utility.hpp
 *
 * Various utility functions and classes.
 *
 **/

#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <functional>
#include <sstream>
#include <iostream>
#include <vector>

#include "assertion.hpp"


template <typename T>
std::string tostr(const std::vector<T>& x) {
  std::ostringstream out;
  bool first= true;
  for (typename std::vector<T>::const_iterator it= x.begin();
		 it != x.end();
		 ++it) {
	 if (!first) {
		out << " ";
	 }
	 out << *it;
	 first= false;
  }
  return out.str();
};

template <typename T>
std::string tostr(const T& x) {
  std::ostringstream ostr;
  ostr << x;
  return ostr.str();
};

template <class T>
class deleter_t
  : public std::unary_function<T*, void> {
public:
  void operator()(T* pt) {
	 delete pt;
  }
};

template <int i, int default_index, class derived_enum>
class static_str_switch {
public:
  static inline derived_enum EXEC(const std::string& s) {
	 if (s == derived_enum::str_values[i]) {
		return derived_enum::enum_values[i];
	 } else {
		return static_str_switch< i-1, default_index, derived_enum>::EXEC(s);
	 }
  }
};

template <int default_index, class derived_enum>
class static_str_switch<0, default_index, derived_enum> {
public:
  static inline derived_enum EXEC(const std::string& s) {
	 if (s == derived_enum::str_values[0]) {
		return derived_enum::enum_values[0];
	 } else {
		MY_FAIL;
		return derived_enum::enum_values[default_index];
	 }
  }
};

template <int i, class derived_enum>
class static_int_switch {
public:
  static inline int EXEC(const int v) {
	 if (v == derived_enum::int_values[i]) {
		return i;
	 } else {
		return static_int_switch< i-1, derived_enum>::EXEC(v);
	 }
  }
};

template <class derived_enum>
class static_int_switch<0, derived_enum> {
public:
  static inline int EXEC(const int v) {
	 if (v == derived_enum::int_values[0]) {
		return 0;
	 } else {
		MY_FAIL;
		return -1;
	 }
  }
};



template <class derived_enum, size_t n_values, int default_index>
class enum_like_t {
private:
  int _d;

protected:

  enum_like_t(const size_t d)
		:_d(d)
  {
	 MY_ASSERT_DBG((0 <= d) && (d<n_values));
  }

  size_t get_ordinal_data() const {
	 return _d;
  }

public:

  enum_like_t(const derived_enum& copy)
		: _d(copy.get_ordinal_data())
  {
	 MY_ASSERT_DBG( copy.get_ordinal_data() <n_values );
  };

  const derived_enum&
  operator=(const derived_enum& copy) {
	 _d= copy.get_ordinal_data();
	 return *this;
  };

  const static size_t N_VALUES= n_values;

  friend bool operator==(const derived_enum& e1, const derived_enum& e2) {
	 return e1.get_ordinal_data() == e2.get_ordinal_data();
  }

  friend bool operator!=(const derived_enum& e1, const derived_enum& e2) {
	 return e1.get_ordinal_data() != e2.get_ordinal_data();
  }

  friend std::ostream& operator<<(std::ostream& out, const derived_enum& val) {
	 return (out << derived_enum::str_values[val.get_ordinal_data()]);
  }

  friend std::istream& operator>>(std::istream& in, derived_enum& val) {
	 char h;
	 if (in >> h) {
		val= static_str_switch< n_values-1, default_index, derived_enum>::EXEC(std::string(1,h));
	 } else {
		MY_FAIL;
		val= derived_enum::enum_values[default_index];
	 }
	 return in;
  }
};

#endif // __UTILITY_HPP__
