/**
 * Copyright 2016 Yan Ivnitskiy
 *
 * TODO: Write tests for this
 */

#include <filter.h>
#include <string.h>

void filter_init_state( filter_state_t *state )
{
  assert(state != NULL);

  memset(&state->values, '\0', sizeof(state->values));
  state->length = 0;
  state->index = 0;
}

filter_value_t filter_add_value( filter_state_t *state, filter_value_t value)
{
  filter_value_t acc;

  if (state->length < FILTER_LEN) {
    state->length++;
  }


  state->values[state->index] = value;
  state->index = (state->index + 1) % FILTER_LEN;

  for (int i = 0; i < FILTER_LEN; i++) {
    acc += state->values[i];
  }

  acc /= (float) state->length;

  return acc;
}
  

#if 0
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
#endif
