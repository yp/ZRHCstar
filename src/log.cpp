#include "log.hpp"

#include <boost/filesystem.hpp>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include "configuration.h"

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace boost::filesystem;

int initialize_logger() {
  static bool configured= false;
  if (configured)
	 return EXIT_SUCCESS;
  int result = EXIT_SUCCESS;
  try {
	 bool file_found= false;
	 path path(current_path());
	 while (!(file_found= exists(path / log_configuration_default_file))
				 && (path != path.root_path())) {
		path= path.parent_path();
	 }
	 if (file_found) {
		try {
		  PropertyConfigurator::configure((path / log_configuration_default_file).string());
		  configured= true;
		} catch (Exception& e) {
		  configured= false;
		}
	 }
	 if (!configured) {
		BasicConfigurator::configure();
		configured= true;
	 }
	 if (!configured) {
		result = EXIT_FAILURE;
	 }
  } catch(Exception&) {
	 result = EXIT_FAILURE;
  }
  return result;
}

