#include "configuration.h"

#include "log.hpp"
#include "assertion.hpp"

int main() {
  initialize_application(APPLICATION_CODENAME " " APPLICATION_VERSION_STRING);

  ASSERT( 0 == 0 );
  ASSERT( 1 == 0 );

}
