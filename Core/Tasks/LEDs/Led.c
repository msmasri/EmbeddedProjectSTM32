/*******************************************************************************
 * @file    Led.c
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

/* Includes ------------------------------------------------------------------*/
#include "Led.h"
/* Private includes ----------------------------------------------------------*/
#include "../../Core/Inc/stm32f446xx.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void InitLed(void);
/* External variables --------------------------------------------------------*/

/**
 * @brief Initialize Led on the board STM32F446RE
 * @note: GPIOA on AHB1 bus
 * 				PIN--> 5
 * @retval none */
static void InitLed(void) {
  /* Enable port A*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* select output mode */
  GPIOA->MODER |= GPIO_MODER_MODE5_0;
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

  /* Output type Output open-drain */
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

  /* Output speed medium*/
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_0;
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5_1;

  /* Pull-up/Down UP*/
  GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_0;
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5_1;

  /**************************** Optional *************************************/
  /* lock configuration */
}

/**
 * @brief User function to turn led on
 * @note:
 * @retval none */
void SetLed(void) { GPIOA->ODR |= GPIO_ODR_OD5; }

/**
 * @brief User function to turn led off
 * @note:
 * @retval none */
void ResetLed(void) { GPIOA->ODR &= ~GPIO_ODR_OD5; }

/**
 * @brief User function to toggle led
 * @note:
 * @retval none */
void Toggle(void) { GPIOA->ODR ^= GPIO_ODR_OD5; }

/**
 * @brief Initialize Led on the board STM32F446RE in main.c
 * @note:
 * @retval none */
void InitMainLed(void) { InitLed(); }
