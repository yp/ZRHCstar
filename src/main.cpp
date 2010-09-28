#include "configuration.h"

#include "log.hpp"

int main() {
  initialize_logger(APPLICATION_CODENAME " " APPLICATION_VERSION_STRING);
}
