/**
 * The hardware abstraction layer functions for micros
 */


enum pinmode_e {
  PINMODE_OUTPUT,
  PINMODE_INPUT,
};

void pin_set(int port, int pin);

void pin_clear(int port, int pin);

void pin_config(int port, int pin, int options);

void spi_config(int port, int options);

void timer_config(int timer, int channel, int options);

void config_isr(int port);
