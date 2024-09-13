/*******************************************************************************
 * @file    DCMotorDriver.h
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

#ifndef TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_H_
#define TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_H_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../../Core/Inc/stm32f446xx.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  uint16_t DutyCycle;
  uint16_t Period;
} PWM_Buffer_Type;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void InitMainDCMotorDriver(void);
void ChangeSpeed(uint16_t *speed);
uint32_t ReadMotorSpeed(void);
/* External variables --------------------------------------------------------*/

#endif /* TASKS_MOTORDRIVERUNIT_DCMOTORDRIVER_H_ */
