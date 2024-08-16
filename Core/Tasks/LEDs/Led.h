/*******************************************************************************
 * @file    Led.h
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

#ifndef TASKS_LEDS_LED_H_
#define TASKS_LEDS_LED_H_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SetLed(void);
void ResetLed(void);
void Toggle(void);
void InitMainLed(void);
/* External variables --------------------------------------------------------*/


#endif /* TASKS_LEDS_LED_H_ */
