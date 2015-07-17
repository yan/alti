/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <filter.h>

void filter_init_state( filter_state_t *state )
{
  assert(state != NULL);

  state->running_avg = 0;
  state->n_values = 0;
}

#define FILTER_LEN    ( 8 )

filter_value_t filter_add_value( filter_state_t *state, filter_value_t value)
{
  float v;
  
  v = (float)state->running_avg * ((float)state->n_values - 1)/state->n_values + (float)value/state->n_values;

  if (state->n_values < 8) {
    state->n_values++;
  }

  state->running_avg = (filter_value_t) v;

  return state->running_avg;
}

filter_value_t filter_get_value( filter_state_t *state )
{
  return state->running_avg;
}
