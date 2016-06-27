/**
 * Copyright 2016 Yan Ivnitskiy
 */

#ifndef __FILTER_H
#define __FILTER_H

#include <stdint.h>
#include <stddef.h>
#include <util.h>

typedef uint32_t filter_value_t;

/**
 * @brief The number of samples in the filter window
 */
#define FILTER_LEN   ( 8 )

/** @brief Average state is just a running average. Average length is a compile-
 * defined constant
 */
typedef struct filter_state_s {
  filter_value_t values[FILTER_LEN];
  uint8_t index;
  uint8_t length;
} filter_state_t;

void filter_init_state( filter_state_t *state );

filter_value_t filter_add_value( filter_state_t *state, filter_value_t value);

// filter_value_t filter_get_value( filter_state_t *state );

#endif // __FILTER_H
