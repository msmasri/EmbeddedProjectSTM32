/*******************************************************************************
 * @file    OutputClock.c
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
#include "OutputClock.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void InitGPIOMCO1(void);
static void ConfigMCO1(void);
static void InitGPIOMCO2(void);
/* External variables --------------------------------------------------------*/
/*
 * note: MCO1
 * PA8,AF0
* rvalue: none
*/
static void InitGPIOMCO1(void)
{
	/* Enable port A*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* select output mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE8_0;
	GPIOA->MODER |= GPIO_MODER_MODE8_1;

	/* Output type Output open-drain */
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

	/* Output speed medium*/
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_0;
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR8_1;

	/* No Pull-up/Down UP*/
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8;

	/* alternative function 1*/
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8;

}

/*
 * note: MCO2
 * PC9,AF0
 * rvalue: none
 */
static void InitGPIOMCO2(void)
{
	/* Enable port C*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	/* select output mode */
	GPIOC->MODER &= ~GPIO_MODER_MODE9_0;
	GPIOC->MODER |= GPIO_MODER_MODE9_1;

	/* Output type Output open-drain */
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT9;

	/* Output speed medium*/
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9_1;

	/* No Pull-up/Down UP*/
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD9;

	/* alternative function 1*/
	GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL9;

}

static void ConfigMCO1(void)
{
	/* divided by 4 */
	RCC->CFGR |= (RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_2);
	RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
//	RCC->CFGR |= RCC_CFGR_MCO1PRE;

	/* PLL clock selected */
	//TODO: Clock source selection may generate glitches on MCO1. It is
	//highly recommended to configure these bits only after reset before enabling the external
	//oscillators and PLL
	RCC->CFGR &= ~RCC_CFGR_MCO1; // HSI
//	RCC->CFGR |= RCC_CFGR_MCO1; //PLL

}

static void ConfigMCO2(void)
{
	/* divided by 4 */
	RCC->CFGR |= (RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2);
	RCC->CFGR &= ~RCC_CFGR_MCO2PRE_0;
//	RCC->CFGR |= RCC_CFGR_MCO2PRE;

	/* PLL clock selected */
	//TODO: Clock source selection may generate glitches on MCO1. It is
	//highly recommended to configure these bits only after reset before enabling the external
	//oscillators and PLL
	RCC->CFGR &= ~RCC_CFGR_MCO2; // System clock (SYSCLK)
//	RCC->CFGR |= RCC_CFGR_MCO2; //PLL
}

void InitMainMCO1(void)
{
	InitGPIOMCO1();
	ConfigMCO1();
}


void InitMainMCO2(void)
{
	InitGPIOMCO2();
	ConfigMCO2();
}
