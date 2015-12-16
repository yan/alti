#if !TESTING
#  error "This file should only be built in test mode"
#endif

#include <aero.h>
#include <logger.h>
#include <flash.h>

int
aero_main(int argc, char *argv[])
{

  (void) argc;
  (void) argv;

  logger_format_storage();
  return 0;
}
