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

#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stm32l1xx_conf.h>

void exti3_isr(void);

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
  // int should_set = 0;

  GPIO_InitTypeDef init = {
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_Pin = pin,
    // .GPIO_Mode = options == PINMODE_INPUT ? GPIO_Mode_IN : GPIO_Mode_OUT,
    .GPIO_PuPd = GPIO_PuPd_DOWN,
    //.GPIO_PuPd = GPIO_PuPd_NOPULL,
    .GPIO_Speed = GPIO_Speed_10MHz
  };

  if (options == PINMODE_INPUT) {
    init.GPIO_Mode = GPIO_Mode_IN;
  } else if (options == PINMODE_OUTPUT) {
    init.GPIO_Mode = GPIO_Mode_OUT;
  } else if (IS_VALID_AF_PINMODE(options)) {
    init.GPIO_Mode = GPIO_Mode_AF;
    // should_set = 1;
    uint8_t pin_source;

    for (pin_source = 0; pin != 0; pin >>= 1, pin_source++) {
      if (pin & 1) {
        GPIO_PinAFConfig( port, pin_source, (uint8_t) options );
      }
    }
  } else {
    assert(0);
  }
  
  GPIO_Init(port, &init);

}

void arch_usart_send(usart_t port, uint16_t data)
{
  while (USART_GetFlagStatus(port, USART_FLAG_TXE) == RESET)
    ;

  USART_SendData(port, data);
}

uint16_t arch_usart_recv(usart_t port)
{
  return USART_ReceiveData(port);
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

  NVIC_SetPriority(EXTI3_IRQn, BLE_EXTI_ISR_PRIORITY);
  NVIC_EnableIRQ(EXTI3_IRQn);

  EXTI_InitTypeDef ble_isr_init = {
    .EXTI_Line = EXTI_Line3,
    .EXTI_LineCmd = ENABLE,
    .EXTI_Mode = EXTI_Mode_Interrupt,
    .EXTI_Trigger = EXTI_Trigger_Falling
  };

  EXTI_Init(&ble_isr_init);
}

/******/

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable.
  * @param  None
  * @retval None
  */
static void SystemInitAero (void)
{
  /*!< Set MSION bit */
  RCC->CR |= (uint32_t)0x00000100;

  /*!< Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], MCOSEL[2:0] and MCOPRE[2:0] bits */
  RCC->CFGR &= (uint32_t)0x88FFC00C;
  
  /*!< Reset HSION, HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xEEFEFFFE;

  /*!< Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /*!< Reset PLLSRC, PLLMUL[3:0] and PLLDIV[1:0] bits */
  RCC->CFGR &= (uint32_t)0xFF02FFFF;

  /*!< Disable all interrupts */
  RCC->CIR = 0x00000000;

#ifdef DATA_IN_ExtSRAM
  // SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM */
    
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE ;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
  SCB->VTOR = FLASH_BASE ;//| VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
}

/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is MSI, SystemCoreClock will contain the MSI 
  *             value as defined by the MSI range.
  *                                   
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32l1xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32l1xx.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
#define SystemCoreClock g.rcc_clock_freq
  const uint8_t PLLMulTable[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};
  const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
  uint32_t tmp = 0, pllmul = 0, plldiv = 0, pllsource = 0, msirange = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  
  switch (tmp)
  {
    case 0x00:  /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
    case 0x04:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x08:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x0C:  /* PLL used as system clock */
      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmul = RCC->CFGR & RCC_CFGR_PLLMUL;
      plldiv = RCC->CFGR & RCC_CFGR_PLLDIV;
      pllmul = PLLMulTable[(pllmul >> 18)];
      plldiv = (plldiv >> 22) + 1;
      
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock selected as PLL clock entry */
        SystemCoreClock = (((HSI_VALUE) * pllmul) / plldiv);
      }
      else
      {
        /* HSE selected as PLL clock entry */
        SystemCoreClock = (((HSE_VALUE) * pllmul) / plldiv);
      }
      break;
    default: /* MSI used as system clock */
      msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
      SystemCoreClock = (32768 * (1 << (msirange + 1)));
      break;
  }
  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
#undef SystemCoreClock
}

static void config_pll(void)
{

  RCC_DeInit();

  /*
   * Enable HSI and wait until ready
   */
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET)
    ;

  FLASH->ACR |= FLASH_ACR_ACC64;        // Enable 64-bit access 
  FLASH->ACR |= FLASH_ACR_PRFTEN;       // Enable Prefetch Buffer 
  FLASH->ACR |= FLASH_ACR_LATENCY;      // Flash 1 wait state 

  /*
   * Enable power and select voltage range 1 (1.8v), then wait until ready
   */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // Enable peripheral clocks
  PWR->CR = PWR_CR_VOS_0;               // Select the Voltage Range 1 (1.8 V) 
  while((PWR->CSR & PWR_CSR_VOSF) != RESET)
    ;

  /*
   * Enable PLL and wait until ready.
   * (HSI = 16MHz; 16MHz * 4 / 2 = 32MHz)
   */
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_3, RCC_PLLDiv_2);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)	// Wait untill PLL is ready
    ;

  RCC_PCLK1Config(RCC_HCLK_Div1);  // Configure the Low Speed APB(APB1) clock (PCLK1).
  RCC_PCLK2Config(RCC_HCLK_Div1);  // Configure the High Speed APB(APB2) clock (PCLK2).
  
  /*
   * Configures the System Clock source to PLL and wait until ready
   */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    ;
}

void arch_config_clocks(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  SystemInitAero();
  // SystemCoreClockUpdate();
  config_pll();

  /* Configure main system clock */
  // RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_3, RCC_PLLDiv_2);


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
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  RCC_GetClocksFreq(&RCC_Clocks);
  g.rcc_clock_freq = RCC_Clocks.SYSCLK_Frequency;

}

void arch_config_uart(usart_t port, int baud)
{
  /* Enable clocks for USART1. */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* AF7 == USART? */
  pin_config(UBLOX_UART_GPIO, UBLOX_UART_PINS, PINMODE_AF_7);

  /* Configure UART for ublox max 7 */

  USART_InitTypeDef USART_InitStruct = {0};
  USART_InitStruct.USART_BaudRate = baud;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(port, &USART_InitStruct);

  /* Clear all flags */
  // USART_ClearFlag(port, ~0); 
  //
  /* Finally enable the USART. */
  USART_Cmd(port, ENABLE);

#if CONFIG_USE_USART_ISR
  arch_enable_usart_interrupt(port);
#endif

}

void arch_disable_usart_interrupt(usart_t port)
{
  USART_ITConfig(port, USART_IT_RXNE, DISABLE);
}

void arch_enable_usart_interrupt(usart_t port)
{
  // make this configurable
  // NVIC_SetPriority(USART1_IRQn, USART_ISR_PRIORITY);
  // NVIC_EnableIRQ(USART1_IRQn);


  NVIC_InitTypeDef NVIC_InitStruct;

  NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = USART_ISR_PRIORITY;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  // TODO: move this to a utility function
  memset(&g.usart_isr_state, '\0', sizeof(g.usart_isr_state));

  USART_ITConfig(port, USART_IT_RXNE, ENABLE);
}


void arch_usart_set_baud(usart_t port, int baud)
{
  arch_config_uart(port, baud);
  return;
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

  if (timer == TIM2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  } else if (timer == TIM4) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  } else {
    assert(0);
  }


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
  SPI_InitTypeDef SPI_InitStruct = {0};

  if (port == SPI1) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  } else if (port == SPI2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  } else {
    assert (0);
  }

  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial = 0; // TODO: What is this??

  SPI_Init(port, &SPI_InitStruct);
  SPI_Cmd(port, ENABLE);
  
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

  // while (!SPI_I2S_GetFlagStatus(port, SPI_I2S_FLAG_RXNE))
    // ;
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
  } else if (port == SPI2) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  } else {
    assert (0);
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

