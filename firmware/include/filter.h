/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __FILTER_H
#define __FILTER_H

#include <stdint.h>
#include <stddef.h>
#include <util.h>

typedef uint32_t filter_value_t;

/** @brief Average state is just a running average. Average length is a compile-
 * defined constant
 */
typedef struct filter_state_s {
  filter_value_t running_avg;
  int n_values;
} filter_state_t;

void filter_init_state( filter_state_t *state );

filter_value_t filter_add_value( filter_state_t *state, filter_value_t value);

filter_value_t filter_get_value( filter_state_t *state );

#endif // __FILTER_H
