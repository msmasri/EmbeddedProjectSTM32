/*******************************************************************************
 * @file    ConfigSystemClock.c
 * @author  sari
 * @email
 * @website
 * @date    Aug 18, 2024
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

/* Includes ------------------------------------------------------------------*/
#include "ConfigSystemClock.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void InitSystemClock(void);
static void InitBusesClock(void);
/* External variables --------------------------------------------------------*/

/*
 * @brief: configure PLL with 180MHz as system clock
 * @note:
 * @retval none
 * */
static void InitSystemClock(void) {
  /* disable pll if it is selected as system clock */
  RCC->CR &= ~RCC_CR_PLLON;

  /* select HSI as clock input for PLL */
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

  /* start configuring pll calculated values */
  // TODO: could make function to select the best values automatically?
  /* to get 180MHz as output it should be chosen as follows
   * PLLN = 360 = 0x168
   * PLLM = 16 = 0x10
   * PPLP = 2 = 0x2
   * */
  RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);  // Reset all old configuration
  RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_5 |
                   RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_8);
  // RCC->PLLCFGR |= (360 << RCC_PLLCFGR_PLLN_Pos);

  RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;  // Reset all old configuration
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;
  // RCC->PLLCFGR |= 16 << RCC_PLLCFGR_PLLM_Pos;

  RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;  // Reset all old configuration

  /* turn PLL on */
  RCC->CR |= RCC_CR_PLLON;

  /* wait until PLL ready */
  while (RCC->CR & RCC_CR_PLLRDY) {
  };

  /* switch to PLL*/
  RCC->CFGR |= RCC_CFGR_SW_HSI;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  /* wait until switching to PLL */
  while (RCC->CFGR & RCC_CFGR_SWS_PLL) {
  };
}

static void InitBusesClock(void) {
  /* select the division factor for I2Ss, SAIs, SYSTEM and SPDIF-Rx clocks
   * driven by PLL clock */
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_1;  // divided by 2

  /* division factor for USB OTG FS, SDIOclocks */
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_2;  // divided by 2
  /* AHB bus, system clock divided by 4 */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV4;

  /* PPRE1 bus AHB clock divided by 4 to get the maximum with 45MHz, if pll is
   * used otherwise, the HSI is used with 16MHz. so 8MHz */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

  /* PPRE2 (high speed) bus AHB clock divided by 2 to get the maximum with 90MHz
   * if pll is used otherwise, the HSI is used with 16MHz. so 8MHz */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
}

void InitMainSysClock(void) {
  InitSystemClock();
  InitBusesClock();
}
