#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#define error_message(args...) fprintf(stderr, args)

#define dbg_print(args...) printf(args)

static int uart_fd = 0;
typedef int usart_t;

void delay_ms(uint32_t ms)
{
  usleep(1000 * ms);
}

/** USART functions */
void arch_usart_send(usart_t port, uint8_t data)
{
  (void) port;
  write(uart_fd, &data, 1);
}

uint16_t arch_usart_recv(usart_t port)
{
  uint16_t ret = 0;
  int n;

  (void) port;

  n = read(uart_fd, &ret, 1);
  if (n > 0) {
    return ret;
  } else {
    for(;;);
  }
}

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 50;           // 5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 50;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes", errno);
}

#include "../src/ublox.c"

int
main(int argc, char *argv[])
{
  char *uart_path = argv[1];

  if (argc < 2) {
    fprintf(stderr, "usage: %s [uart dev]\n", argv[0]);
    return 1;
  }

  printf("about to open '%s'\n", uart_path);
  uart_fd = open(uart_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (uart_fd < 0) {
    fprintf(stderr, "failed opening %s: %s\n", uart_path, strerror(errno));
    return 3;
  }

  if (ioctl(uart_fd, TIOCEXCL) == -1) {
      printf("Error setting TIOCEXCL on %s - %s(%d).\n",
          uart_path, strerror(errno), errno);
      return 3;
  }

  if (fcntl(uart_fd, F_SETFL, 0) == -1)
  {
      printf("Error clearing O_NONBLOCK %s - %s(%d).\n",
          uart_path, strerror(errno), errno);
      return 3;
  }
  
  printf("opened\n");

  set_interface_attribs(uart_fd, B9600, 0);
  printf("set attribs\n");
  set_blocking(uart_fd, 1);
  printf("set blocking\n");

  printf("about to read..\n");

  ublox_init();
  ublox_set_measuring_rate(200 /*ms*/);
  ublox_start_updates(1);

  while (1) {
    ublox_get();
  }

  return 0;
}
