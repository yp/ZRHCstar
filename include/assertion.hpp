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
 * assertion.hpp
 *
 * Class to express run-time assertions.
 *
 **/
#ifndef __ASSERTION_HPP__
#define __ASSERTION_HPP__

#include "log.hpp"

#include <log4cxx/logger.h>
#include <boost/preprocessor/stringize.hpp>

class assertion_failed_exception {
private:
  std::string _msg;
public:
  assertion_failed_exception(void)
		:_msg("Assertion failed!")
  {}

  assertion_failed_exception(const std::string& s)
		:_msg(s)
  {}

  ~assertion_failed_exception(void) {
  }

  const std::string& what(void) const {
	 return _msg;
  }
};



class asserter_t {

private:

  log4cxx::LoggerPtr logger;

  asserter_t(void)
		:logger(log4cxx::Logger::getLogger("assertions"))
  {
	 initialize_logger();
  }

public:
  static const asserter_t& get_asserter(void) {
	 static asserter_t asserter;
	 return asserter;
  }

  void basic_assert(const bool condition) const {
	 if (!condition) {
		FATAL("Assertion violated!");
		throw assertion_failed_exception();
	 }
  }

  void assert_msg(const bool condition, const std::string& msg) const {
	 if (!condition) {
		FATAL(msg);
		throw assertion_failed_exception(msg);
	 }
  }
};

#define DESCRIBE __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__)

#ifdef NO_DBG_ASSERTIONS

#define MY_ASSERT_DBG( cond ) while (false) {}

#else // not defined NO_DBG_ASSERTIONS

#define MY_ASSERT_DBG( cond ) MY_ASSERT( cond )

#endif // NO_DBG_ASSERTIONS

#define MY_ASSERT( cond ) if (!(bool)(cond)) { asserter_t::get_asserter().assert_msg( (cond), "Assertion " BOOST_PP_STRINGIZE( cond ) " failed at file " DESCRIBE); }


#define MY_FAIL asserter_t::get_asserter().assert_msg( (false), "Unexpected control-flow at file " DESCRIBE)

#define MY_CHECK( cond ) do {								\
  DEBUG("Checking " BOOST_PP_STRINGIZE(cond) "...");	\
  MY_ASSERT(cond);													\
  } while (0)


#endif // __ASSERTION_HPP__
