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
	 throw assertion_failed_exception(_msg);
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


#define ASSERT( cond ) asserter_t::get_asserter().assert_msg( (cond), "Assertion " BOOST_PP_STRINGIZE( cond ) " failed at file " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__))

#endif // __ASSERTION_HPP__
