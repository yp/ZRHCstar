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
 * descriptive_class.hpp
 *
 * Interface for classes that describe themselves.
 *
 **/

#ifndef __DESCRIPTIVE_CLASS_HPP__
#define __DESCRIPTIVE_CLASS_HPP__

#include <string>

class descriptive_class {

protected:

  virtual const std::string get_brief_description_impl() const = 0;

  virtual const std::string get_long_description_impl() const = 0;

  virtual const std::string get_format_impl() const = 0;

public:
  const std::string get_brief_description() const {
    return get_brief_description_impl();
  }

  const std::string get_long_description() const {
    return get_long_description_impl();
  }

  const std::string get_format() const {
    return get_format_impl();
  }

};

#endif // __DESCRIPTIVE_CLASS_HPP__
