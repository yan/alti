#include <aero.h>
#include "test_native/test_native.h"

int main(int argc, char *argv[])
{
#ifdef TESTING_NATIVE
    return test_native(argc, argv);
#else
    return aero_main(argc, argv);
#endif
}
