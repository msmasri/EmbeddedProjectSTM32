/*******************************************************************************
 * @file    DCMotorDriver.c
 * @author  sari
 * @email
 * @website
 * @date    Aug 16, 2024
 *
 * @brief
 * @note
 *
 @verbatim
 Copyright (C) on GitHub msmasri/stm32project, 2024

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>.
 @endverbatim
 *******************************************************************************/

#ifndef TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_C_
#define TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_C_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "DCMotorDriver.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* 		f_ck_PRS				45MHz
 *-----------------=--------= 1MHz
 *		PRS[15:0] + 1			45
 */
#define PRE_SCALE_VALUE 0x6
#define AUTO_RELOAD_VALUE 0xFFFF
#define COMPARE_VALUE (AUTO_RELOAD_VALUE / 2)  // for 50% duty cycle

/* Private macro -------------------------------------------------------------*/
#define NUMBER_OF_DATA_ITEMS 2

/* Private variables ---------------------------------------------------------*/
static motorData_type measuredMotorPWM;

/* Private function prototypes -----------------------------------------------*/
static void InitGPIOMotorDriver(void);
static void InitPWMEncoderDriver(void);
static void InitPWMMotorDriver(void);
static void InitGPIOEncoderDriver(void);
static void InitDMAEncoderDriver(void);
static void SetDutyCycle(uint16_t dutyCycle);
// static void InitDMAMotorDriver(void);
/* External variables --------------------------------------------------------*/

/*
 * note: TIM2_CH1
 * 			 AF1, PORTA, PA1
 * rvalue: none
 */
static void InitGPIOMotorDriver(void) {
  /* Enable port A*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* select Alternative function mode */
  GPIOA->MODER &= ~GPIO_MODER_MODE1_0;
  GPIOA->MODER |= GPIO_MODER_MODE1_1;

  /* Output type Output open-drain */
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT1;

  /* Output speed medium*/
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR1_0;
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR1_1;

  /* Pull-up/Down UP*/
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;

  /* alternative function 1*/
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL1_0;
  GPIOA->AFR[0] &=
      ~(GPIO_AFRL_AFSEL1_1 | GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_3);
}

/*
 * note: TIM2_CH1
 * rvalue: none
 */
static void InitPWMMotorDriver(void) {
  /* enable timer 2 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // TODO: should set the value of the pre-scale and auto-compare and autoreload
  // to get 50 Hz
  TIM2->PSC = PRE_SCALE_VALUE;

  /* set the auto-reload value*/
  TIM2->ARR = AUTO_RELOAD_VALUE;

  /* captuer/compare value */
  TIM2->CCR2 = COMPARE_VALUE;

  /* by changing the compare value is should be direct change register value*/
  TIM2->CR1 &= ~TIM_CR1_ARPE;

  /* Center-aligned mode, cc2s is set both up/down counting cases */
  TIM2->CR1 |= TIM_CR1_CMS;

  /* signal is active on the corresponding output pin */
  TIM2->CCER |= TIM_CCER_CC2E;

  /* OC1 polarity on high*/
  TIM2->CCER |= TIM_CCER_CC2P;

  /*CC2 channel is configured as output */
  TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;

  /* Output compare 2 as PWM mode 1*/
  TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_0;
  TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

  /* disable pre-load to take the new value in account immediately*/
  TIM2->CCMR1 &= ~TIM_CCMR1_OC2PE;

  /* enable fast */
  TIM2->CCMR1 |= TIM_CCMR1_OC2FE;

  /* initialize all register */
  TIM2->EGR |= TIM_EGR_UG;

  /* Enable timer */
  TIM2->CR1 |= TIM_CR1_CEN;
}

/*
 * note: TIM3_CH1
 * 			 AF2, PORTA, PA6
 * rvalue: none
 */
static void InitGPIOEncoderDriver(void) {
  /* Enable port A*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* select output mode */
  GPIOA->MODER &= ~GPIO_MODER_MODE6_0;
  GPIOA->MODER |= GPIO_MODER_MODE6_1;

  /* Output type Output open-drain */
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

  /* Output speed medium*/
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6_1;

  /* Pull-up/Down UP*/
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;

  /* alternative function 1*/
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL6_1;
  GPIOA->AFR[0] &=
      ~(GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_3);
}

/*
 *@note: TIM3_CH1
 * 			 AF2, PORTA, PA6
 *@rvalue: none
 */
static void InitPWMEncoderDriver(void) {
  /* enable timer 3 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  TIM3->PSC = PRE_SCALE_VALUE;

  /* set the auto-reload value*/
  TIM3->ARR = 0xFFFF;

  /* Select the active input for TIMx_CCR1: write the CC1S bits to 01 in the
   * TIMx_CCMR1 register (TI1 selected)*/
  TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
  TIM3->CCMR1 &= ~TIM_CCMR1_CC1S_1;

  /*Select the active input for TIMx_CCR2: write the CC2S bits to 10 in the
   TIMx_CCMR1 register (TI1 selected).*/
  TIM3->CCMR1 &= ~TIM_CCMR1_CC2S_0;
  TIM3->CCMR1 |= TIM_CCMR1_CC2S_1;

  /* Select the active polarity for TI1FP1 (used both for capture in TIMx_CCR1
   * and counter clear ): write the CC1P to ‘0’ and the CC1NP bit to ‘0’ (active
   * on rising edge).*/
  TIM3->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);

  /* Select the active polarity for TI1FP2 (used for capture in TIMx_CCR2):
   write the CC2P bit to ‘1’ and the CC2NP bit to ’0’(active on falling edge).
 */
  TIM3->CCER |= TIM_CCER_CC2P;
  TIM3->CCER &= ~TIM_CCER_CC2NP;

  /* Select the valid trigger input: write the TS bits to 101 in the TIMx_SMCR
   register (TI1FP1 selected). */
  TIM3->SMCR |= (TIM_SMCR_TS_2 | TIM_SMCR_TS_0);
  TIM3->SMCR &= ~TIM_SMCR_TS_1;

  /* Configure the slave mode controller in reset mode: write the SMS bits to
   100 in the TIMx_SMCR register. */
  TIM3->SMCR |= TIM_SMCR_SMS_2;
  TIM3->SMCR &= ~(TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);

  /* Enable the captures: write the CC1E and CC2E bits to ‘1 in the TIMx_CCER
   * register. */
  TIM3->CCER |= TIM_CCER_CC1E;
  TIM3->CCER |= TIM_CCER_CC2E;

  /* initialize all register */
  TIM3->EGR |= TIM_EGR_UG;

  /* enable DMA burst to 2 transfer */
  TIM3->DCR &= ~TIM_DCR_DBL;  // clear complete register first
  TIM3->DCR |= TIM_DCR_DBL_0;

  /* starting from address 13=1101 which is CCR1 */
  TIM3->DCR &= ~TIM_DCR_DBA;
  TIM3->DCR |= (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_0);

  /* Enable Capture/Compare 1 DMA request enable */
  TIM3->DIER |= TIM_DIER_CC1DE;
  TIM3->DIER |= TIM_DIER_CC2DE;

  /* Select DMA burst length = 2 */
  TIM3->DCR &= ~TIM_DCR_DBL;
  TIM3->DCR |= TIM_DCR_DBL_0;

  /* Enable timer */
  TIM3->CR1 |= TIM_CR1_CEN;
}
// DMA1, channel 5 stream 4 for TIM3_CH1
static void InitDMAEncoderDriver(void) {
  /* enable DMA1 port*/
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  /* select channel 5 */
  DMA1_Stream4->CR |= (DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_2);
  DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL_1;

  /* data size in memory and peripheral half-word */
  DMA1_Stream4->CR |= DMA_SxCR_MSIZE_0;
  DMA1_Stream4->CR |= DMA_SxCR_PSIZE_0;

  /* enable circular mode*/
  DMA1_Stream4->CR |= DMA_SxCR_CIRC;

  /* enable circular mode*/
  DMA1_Stream4->NDTR = NUMBER_OF_DATA_ITEMS;

  /* Select stream priority very high */
  DMA1_Stream4->CR |= DMA_SxCR_PL;

  /* peripheral address */
  DMA1_Stream4->PAR = (uint32_t)(&(TIM3->DMAR));

  /* memory address */
  DMA1_Stream4->M0AR = (uint32_t)(&measuredMotorPWM);

  /* direction is peripheral to memory */
  DMA1_Stream4->CR &= ~DMA_SxCR_DIR;

  /* increase memory address*/
  DMA1_Stream4->CR |= DMA_SxCR_MINC;

  /* fixed peripheral address*/
  DMA1_Stream4->CR &= ~DMA_SxCR_PINC;

  NVIC_SetPriority(DMA1_Stream4_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Stream4_IRQn);

  /* enable stream */
  DMA1_Stream4->CR |= DMA_SxCR_EN;
}

void InitMainDCMotorDriver(void) {
  InitGPIOMotorDriver();
  InitPWMMotorDriver();

  /* Initialize encoder unit*/
  InitGPIOEncoderDriver();
  InitPWMEncoderDriver();
  InitDMAEncoderDriver();
}

/* =============================================================================
 *
                        API For DC Motor Driver
 =============================================================================*/
static void SetDutyCycle(uint16_t dutyCycle) {}

// TODO: uint32_t or uint16_t? check register length
static uint32_t ReadEncoderValue(void) { return 0; }

// TODO: uint32_t or uint16_t? check register length
uint32_t ReadMotorSpeed(void) {
  uint32_t tempValue = ReadEncoderValue();
  // TODO: the tempValue should be converted to physical value
  return tempValue;
}

void ChangeSpeed(uint16_t *speed) {
  // TODO: convert the argument value to PWM duty cycle
  uint16_t dutyCycle = 10;
  SetDutyCycle(dutyCycle);
}

#endif /* TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_C_ */
