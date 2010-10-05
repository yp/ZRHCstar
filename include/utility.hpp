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

template <class T>
class deleter_t
  : public std::unary_function<T*, void> {
public:
  void operator()(T* pt) {
	 delete pt;
  }
};

template <int i, int default_index, class derived_enum>
class static_switch {
public:
  static inline derived_enum EXEC(const char c) {
	 if (c == '0'+derived_enum::values[i]) {
		return derived_enum::enum_values[i];
	 } else {
		return static_switch< i-1, default_index, derived_enum>::EXEC(c);
	 }
  }
};

template <int default_index, class derived_enum>
class static_switch<0, default_index, derived_enum> {
public:
  static inline derived_enum EXEC(const char c) {
	 if (c == '0'+derived_enum::values[0]) {
		return derived_enum::enum_values[0];
	 } else {
		MY_FAIL;
		return derived_enum::enum_values[default_index];
	 }
  }
};



template <class derived_enum, int n_values, int default_index>
class enum_like_t {
private:
  int _d;

protected:

  enum_like_t(const int& d)
		:_d(d)
  {}

  int get_data() const {
	 return _d;
  }

public:
  friend bool operator==(const derived_enum& e1, const derived_enum& e2) {
	 return e1._d == e2._d;
  }

  friend bool operator!=(const derived_enum& e1, const derived_enum& e2) {
	 return e1._d != e2._d;
  }

  friend std::ostream& operator<<(std::ostream& out, const derived_enum& val) {
	 return (out << val._d);
  }

  friend std::istream& operator>>(std::istream& in, derived_enum& val) {
	 char h;
	 if (in >> h) {
		val= static_switch< n_values-1, default_index, derived_enum>::EXEC(h);
	 } else {
		MY_FAIL;
		val= derived_enum::enum_values[default_index];
	 }
	 return in;
  }
};

#endif // __UTILITY_HPP__
