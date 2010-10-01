#include "configuration.h"

#include "application.hpp"
#include "log.hpp"
#include "assertion.hpp"


class zrhcstar_application_t: public application_t {

public:

  zrhcstar_application_t()
		:application_t(APPLICATION_CODENAME " " APPLICATION_VERSION_STRING)
  {}

protected:

  virtual int execution(int argc, char** argv) {

	 return EXIT_SUCCESS;
  }

};



int main(int argc, char** argv) {
  zrhcstar_application_t app;
  return app.execute(argc, argv);
}
