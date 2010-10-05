#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "log.hpp"
#include "assertion.hpp"

class application_t {

protected:

  std::string _name;
  log4cxx::LoggerPtr logger;

  virtual int execution(int argc, char** argv) = 0;

public:
  explicit application_t(const std::string& name)
		:_name(name),
		 logger(log4cxx::Logger::getLogger("application"))
  {
	 initialize_logger();
  }

  virtual ~application_t() {
  }

  int execute(int argc, char** argv) {
	 boost::posix_time::ptime time_start(boost::posix_time::second_clock::local_time());
	 INFO(_name << " - started at " << time_start);
	 DEBUG("Source code version: " APPLICATION_SOURCE_VERSION);
	 DEBUG("Compiled on:         " __DATE__ "  -  " __TIME__);

	 int result= EXIT_SUCCESS;
	 try {
		result= execution(argc, argv);
	 } catch (std::exception& e) {
		FATAL("Exception occurred: " << e.what() << ".");
		result= EXIT_FAILURE;
	 } catch (assertion_failed_exception& e) {
		FATAL("Assertion failed: " << e.what() << ".");
		result= EXIT_FAILURE;
		throw e;
	 } catch (...) {
		FATAL("Generic exception occurred.");
		result= EXIT_FAILURE;
	 }

	 boost::posix_time::ptime time_end(boost::posix_time::second_clock::local_time());
	 DEBUG("Running time:        " << (time_end-time_start));
	 INFO(_name << " - terminated at " << time_end);

	 return result;
  }

};



#endif //__APPLICATION_HPP__
