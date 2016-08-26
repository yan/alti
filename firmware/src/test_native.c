
#ifndef TESTING_NATIVE
# error "Must have TESTING_NATIVE enabled for native tests"
#endif

#include "test_native/test_native.h"
#include <logger.h>
#include <flash.h>
#include <rtos.h>
#include <hal.h>
#include <events.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <periph.h>
#include <buffered_io.h>

static void config_all(void)
{
    arch_config_clocks();
    arch_config_nvic();
    arch_config_io();
  
    config_globals();
    config_flash();
}

static int test_logger(void)
{
    struct event_s event;
    struct sensor_packet_s packet;

    const size_t kPacketsToWrite = 256;
    size_t i = 0;

    logger_format_storage();
    logger_start_event(&event);
    for (i = 0; i < kPacketsToWrite; i++) {
        packet.ticks = i;
        logger_write_sample(&event, &packet);
    }
    logger_end_event(&event);

    buffered_flush();

    logger_get_event(NULL, &event);
    
    for (i = 0; logger_read_sample(&event, i, &packet); i++) {
        assert(packet.ticks == i);
    }

    assert(0);
    
    return 1;
}

int test_native(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    config_all();

    test_logger();

    return 1;
}
