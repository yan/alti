/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifdef STM32_STDPERIPH_LIB

#include <hal.h>
#include <nrf8001.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <pins.h>
#include <spi.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stm32l1xx_conf.h>

void __attribute__((weak)) exti3_isr(void);

void pin_set(gpio_t port, pin_t pin)
{
  GPIO_SetBits(port, pin);
}

void pin_clear(gpio_t port, pin_t pin)
{
  GPIO_ResetBits(port, pin);
}

void pin_toggle(gpio_t port, pin_t pin)
{
  GPIO_ToggleBits(port, pin);
}

void pin_config(gpio_t port, pin_t pin, int options)
{
  GPIO_InitTypeDef init = {
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_Pin = pin,
    .GPIO_Mode = options == PINMODE_INPUT ? GPIO_Mode_IN : GPIO_Mode_OUT,
    .GPIO_PuPd = GPIO_PuPd_NOPULL,
    .GPIO_Speed = GPIO_Speed_10MHz
  };

  assert(options == PINMODE_INPUT || options == PINMODE_OUTPUT);

  GPIO_Init(port, &init);
}

void arch_usart_send(usart_t port, uint8_t data)
{
  (void) port;
  (void) data;
}

uint16_t arch_usart_recv(usart_t port)
{
  (void) port;
  return 0;
}
void spi_config(spi_t port, int options)
{
  (void) port;
  (void) options;
}

void timer_config(pwm_timer_t timer, int channel, int options)
{
  (void) timer;
  (void) channel;
  (void) options;
}

void arch_config_ble(void)
{
  // rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SYSCFGEN);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  NVIC_EnableIRQ(EXTI3_IRQn);
  NVIC_SetPriority(EXTI3_IRQn, BLE_EXTI_ISR_PRIORITY);

  EXTI_InitTypeDef ble_isr_init = {
    .EXTI_Line = EXTI_Line3,
    .EXTI_LineCmd = ENABLE,
    .EXTI_Mode = EXTI_Mode_Interrupt,
    .EXTI_Trigger = EXTI_Trigger_Falling
  };

  EXTI_Init(&ble_isr_init);
}

void arch_config_clocks(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* Configure main system clock */
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_3, RCC_PLLDiv_2);

  /* Enable peripheral clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Enable LSE */
  /* Enable access to the RTC XXX: Do we need to disable it? */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* Enable LSE and wait until it's ready */
  RCC_LSEConfig(RCC_LSE_ON);
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET)
    ;

  /* Set the RTC to use the LSE */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  RCC_GetClocksFreq(&RCC_Clocks);
  g.rcc_clock_freq = RCC_Clocks.SYSCLK_Frequency;

}

void arch_config_uart(usart_t port, int baud)
{
  USART_InitTypeDef USART_InitStruct;
  /* Enable clocks for USART1. */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* AF7 == USART? */
  pin_config(UBLOX_UART_GPIO, UBLOX_UART_PINS, PINMODE_AF_7);

  /* Configure UART for ublox max 7 */
  USART_InitStruct.USART_BaudRate = baud;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(port, &USART_InitStruct);

  /* Finally enable the USART. */
  USART_Cmd(port, ENABLE);
}

void arch_usart_set_baud(usart_t port, int baud)
{
  /* Straight from stm32l1xx_usart.c */

  RCC_ClocksTypeDef RCC_Clocks;

  uint16_t tmpreg = port->BRR;
  uint32_t apbclock = 0x00;
  uint32_t integerdivider = 0x00;
  uint32_t fractionaldivider = 0x00;

  RCC_GetClocksFreq(&RCC_Clocks);
  if (port == USART1) 
  {
    apbclock = RCC_Clocks.PCLK2_Frequency;
  }
  else
  {
    apbclock = RCC_Clocks.PCLK1_Frequency;
  }
  
  /* Determine the integer part */
  if ((port->CR1 & USART_CR1_OVER8) != 0)
  {
    /* Integer part computing in case Oversampling mode is 8 Samples */
    integerdivider = ((25 * apbclock) / (2 * (baud)));    
  }
  else /* if ((port->CR1 & CR1_OVER8_Set) == 0) */
  {
    /* Integer part computing in case Oversampling mode is 16 Samples */
    integerdivider = ((25 * apbclock) / (4 * (baud)));    
  }
  tmpreg = (integerdivider / 100) << 4;
  
  /* Determine the fractional part */
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
  
  /* Implement the fractional part in the register */
  if ((port->CR1 & USART_CR1_OVER8) != 0)
  {
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
  }
  else /* if ((port->CR1 & CR1_OVER8_Set) == 0) */
  {
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  }
 
  /* Write to USART BRR */
  port->BRR = (uint16_t)tmpreg;
}

void arch_config_nvic(void)
{
  /* 4 bits for preemption priority, 0 for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void exti3_isr(void)
{
  /* exti_reset_request(EXTI_PR & EXTI3); */
  EXTI_ClearITPendingBit(EXTI_Line3);

  nrf8001_isr();
}

void arch_init_timer(pwm_timer_t timer, uint32_t channel, uint32_t prescaler, uint32_t period)
{
  TIM_TimeBaseInitTypeDef TIM_Struct;
  TIM_OCInitTypeDef TIM_OCStruct;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_Struct.TIM_Prescaler = prescaler;
  TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_Struct.TIM_Period = period;
  TIM_Struct.TIM_ClockDivision = 0;

  TIM_TimeBaseInit(timer, &TIM_Struct);

  TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCStruct.TIM_Pulse = 0;
  TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;

  switch (channel) {
    case 1:
      TIM_OC1Init(timer, &TIM_OCStruct);
      break;
    case 2:
      TIM_OC2Init(timer, &TIM_OCStruct);
      break;
    case 3:
      TIM_OC3Init(timer, &TIM_OCStruct);
      break;
    case 4:
      TIM_OC4Init(timer, &TIM_OCStruct);
      break;
  }
}

void arch_timer_set(pwm_timer_t timer, uint32_t channel, uint32_t value)
{
  switch (channel) {
    case 1:
      TIM_SetCompare1(timer, value);
      break;
    case 2:
      TIM_SetCompare2(timer, value);
      break;
    case 3:
      TIM_SetCompare3(timer, value);
      break;
    case 4:
      TIM_SetCompare4(timer, value);
      break;
  }
}

/**
 * @brief Configure and enable SPI1 or SPI2. Initialize it with a default
 * byte order, although this can change on a per-message basis.
 *
 * @param port 1 for SPI1 or 2 for SPI2
 * @param byte_order 0 for LSB first, 1 for MSB first
 */
void arch_spi_config(spi_t port)
{
  SPI_InitTypeDef SPI_InitStruct;

  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CRCPolynomial = 0; // TODO: What is this??
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

  SPI_Init(port, &SPI_InitStruct);
  
  arch_spi_enable(port);
}


void spi_set_msb(spi_t port)
{
  port->CR1 &= ~SPI_FirstBit_LSB;
}

void spi_set_lsb(spi_t port)
{
  port->CR1 |= SPI_FirstBit_LSB;
}

/**
 * @brief
 */
uint8_t arch_spi_xfer(spi_t port, uint8_t cmd)
{
  SPI_I2S_SendData(port, cmd);

  while (!(port->SR & SPI_I2S_FLAG_RXNE))
    ;

  return SPI_I2S_ReceiveData(port);

}

/**
 * @brief Enable the SPI peripheral.
 *
 * @param port 1 for SPI1, 2 for SPI2
 */
void arch_spi_enable(spi_t port)
{
  if (port == SPI1) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  } else {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  }

  SPI_Cmd(port, ENABLE);
}

void timer_disable(pwm_timer_t timer)
{
  if (timer == TIM2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
  } else if (timer == TIM4) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
  }

  TIM_Cmd(timer, DISABLE);
}

void timer_enable(pwm_timer_t timer)
{
  if (timer == TIM2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  } else if (timer == TIM4) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  }
  TIM_Cmd(timer, ENABLE);
}
#endif // ifdef STM32_STDPERIPH_LIB

