/*******************************************************************************
 * @file    RTU.h
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

#ifndef TASKS_REALTIMEUNIT_RTU_H_
#define TASKS_REALTIMEUNIT_RTU_H_

/* Includes ------------------------------------------------------------------*/
#include "../../Core/Inc/stm32f446xx.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t timeformat;
	uint16_t subsecond;
} TimeType;
typedef struct
{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t weekday;
} DateType;

typedef struct{
	TimeType tm;
	DateType dt;
}TimeDateType;
/* Private function prototypes -----------------------------------------------*/

/* External variables --------------------------------------------------------*/
void InitMainRTU(void);
void ReadCalender(TimeDateType *pTD);

#endif /* TASKS_REALTIMEUNIT_RTU_H_ */
