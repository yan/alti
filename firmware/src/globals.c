/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <globals.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
  
struct globals g = {0};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

#ifdef __cplusplus
}
#endif
