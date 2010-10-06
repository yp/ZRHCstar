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

  bool is_male() {
	 return _g == _MALE;
  }

  bool is_female() {
	 return _g == _FEMALE;
  }

  bool is_not_specified() {
	 return _g == _UNSPEC;
  }

  bool is_not_male() {
	 return _g != _MALE;
  }

  bool is_not_female() {
	 return _g != _FEMALE;
  }

  bool is_specified() {
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

};


#endif // __GENDER_HPP__
