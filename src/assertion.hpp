/**
 *
 * assertion.hpp
 *
 * Class to express run-time assertions.
 *
 **/

#include <log4cxx/logger.h>
#include <boost/preprocessor/stringize.hpp>

using namespace log4cxx;

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

  LoggerPtr logger;

  asserter_t(void)
		:logger(Logger::getLogger("assertions"))
  {
	 initialize_logger();
  }

public:
  static const asserter_t& get_asserter(void) {
	 static asserter_t asserter;
	 return asserter;
  }

  void assert(const bool condition) const {
	 if (!condition) {
		LOG4CXX_FATAL(logger, "Assertion violated!");
		throw assertion_failed_exception();
	 }
  }

  void assert_msg(const bool condition, const std::string& msg) const {
	 if (!condition) {
		LOG4CXX_FATAL(logger, msg);
		throw assertion_failed_exception(msg);
	 }
  }
};


#define ASSERT( cond ) asserter_t::get_asserter().assert_msg( (cond), "Assertion " BOOST_PP_STRINGIZE( cond ) " failed at file " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__))
