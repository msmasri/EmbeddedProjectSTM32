/*******************************************************************************
 * @file    RTU.c
 * @author  sari
 * @email   
 * @website 
 * @date    Sep 1, 2024
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
#include "RTU.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define PRETECION_KEY_1							0xCA
#define PRETECION_KEY_2							0x53
#define PRER_PREDIV_A								0x7C //124
#define PRER_PREDIV_S								0xFF //255
#define ENABLE_WRITE_PROTECTION 		0xFF
/* Private variables ---------------------------------------------------------*/
TimeDateType buffer;
static float Secondfraction = 0;

/* Private function prototypes -----------------------------------------------*/
static void InitRTU(void);
static void RTCCalibration(void);
static uint8_t RTC_Bcd2ToByte(uint8_t number);
/* External variables --------------------------------------------------------*/

static void InitRTU(void)
{
	/* enable LSI clock */
	RCC->CSR |= RCC_CSR_LSION;

	/* wait until LSI is ready */
	while ((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY)
	{
	}

	/* unlock write protection for backup domain control register */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;

	/* enable clock for RTC */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/* select LSI = 32kHz to clock RTC. this is different a bit to external crystal which has
	 * 32,768kHz */
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;

	/***************************************/
	/* (7) Disable write access for RTC registers */

	/* unlock protection registers for RTC */
	RTC->WPR = PRETECION_KEY_1;
	RTC->WPR = PRETECION_KEY_2;

	/* select initialization mode */
	RTC->ISR |= RTC_ISR_INIT;

	while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF)
	{
	}

	/* pre-scale clock of RTU to 1 Hz */
	RTC->PRER = 0x007C00FF;

	/* run without any initial time and date for now //TODO: write a initial time and date */

	/* select 24-hour format */
	RTC->TR &= ~RTC_TR_PM;

	/* select running free mode */
	RTC->ISR &= ~ RTC_ISR_INIT;

	/* lock write protection for backup domain control register */
	PWR->CR &= ~PWR_CR_DBP;

	RTC->WPR = ENABLE_WRITE_PROTECTION;
}

/**
 * @brief this function is to convert the reading register to byte format
 * @note: this function is original from st
 * @retval none */
static uint8_t RTC_Bcd2ToByte(uint8_t number)
{
	uint32_t tens = 0U;
	tens = (((uint32_t) number & 0xF0U) >> 4U) * 10U;
	return (uint8_t) (tens + ((uint32_t) number & 0x0FU));
}

/**
 * @brief this function is to read the time and date for the real time unit on the board STM32F446RE
 * @note: the format of the time is 24-Hour format.
 * @retval none */
void ReadCalender(TimeDateType *pTD)
{
	if ((RTC->ISR & RTC_ISR_RSF) == RTC_ISR_RSF)
	{

		pTD->tm.sec = (uint8_t) RTC_Bcd2ToByte(((RTC->TR & (RTC_TR_ST | RTC_TR_SU))));
		pTD->tm.min = (uint8_t) RTC_Bcd2ToByte(
				((RTC->TR & (RTC_TR_MNT | RTC_TR_MNU)) >> RTC_TR_MNU_Pos));
		pTD->tm.hour = (uint8_t) RTC_Bcd2ToByte(((RTC->TR & (RTC_TR_HT | RTC_TR_HU)) >> RTC_TR_HU_Pos));
		pTD->tm.timeformat = (uint8_t) RTC_Bcd2ToByte(((RTC->TR & (RTC_TR_PM)) >> RTC_TR_PM_Pos));

		/* Second fraction = ( PREDIV_S - SS ) / ( PREDIV_S + 1 ) */
		pTD->tm.subsecond = (uint16_t) ((RTC->SSR & (RTC_SSR_SS)));
		float tmp1 = (float) (PRER_PREDIV_S - pTD->tm.subsecond);
		float tmp2 = (float) (PRER_PREDIV_S + 1);
		Secondfraction = tmp1 / tmp2;

		pTD->dt.year = (uint8_t) RTC_Bcd2ToByte(((RTC->DR & (RTC_DR_YT | RTC_DR_YU)) >> RTC_DR_YU_Pos));
		pTD->dt.month = (uint8_t) RTC_Bcd2ToByte(
				((RTC->DR & (RTC_DR_MT | RTC_DR_MU)) >> RTC_DR_MU_Pos));
		pTD->dt.day = (uint8_t) RTC_Bcd2ToByte(((RTC->DR & (RTC_DR_DT | RTC_DR_DU))));
		pTD->dt.weekday = (uint8_t) RTC_Bcd2ToByte(((RTC->DR & (RTC_DR_WDU)) >> RTC_DR_WDU_Pos));
	} else
	{
		/* do nothing */
	}

}

/**
 * @brief this function is to calibrate the real time unit on the board STM32F446RE
 * @note: this function use the LSI clock with 32kHz.
 * @retval none */
static void RTCCalibration(void)
{
	/* unlock protection registers for RTC */
	RTC->WPR = PRETECION_KEY_1;
	RTC->WPR = PRETECION_KEY_2;

	/* select initialization mode */
	RTC->ISR |= RTC_ISR_INIT;
	while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF)
	{
	}

	/* pre-scale clock of RTU to 1 Hz */
	RTC->PRER = 0x007C00FF;

	/* select 24-hour format */
	RTC->TR &= ~RTC_TR_PM;

	/* select running free mode */
	RTC->ISR &= ~ RTC_ISR_INIT;

	while ((RTC->ISR & RTC_ISR_RECALPF) == RTC_ISR_RECALPF)
	{
	}
	/* add your calibration value */
	uint32_t yourvalue = 0x1;
	RTC->CALR = RTC_CALR_CALP | yourvalue;

	RTC->WPR = ENABLE_WRITE_PROTECTION;
}

void InitMainRTU(void)
{
	InitRTU();
	RTCCalibration();
}
