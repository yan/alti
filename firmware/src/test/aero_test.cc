#include <string.h>
#include <aero.h>
#include <logger.h>
#include <flash.h>

#include <gtest/gtest.h>

int
aero_main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
