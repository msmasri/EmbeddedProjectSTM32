/*******************************************************************************
 * @file    USART.h
 * @author  sari
 * @email
 * @website
 * @date    Aug 17, 2024
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

#ifndef TASKS_COMMUNICATION_WITHPC_USART_H_
#define TASKS_COMMUNICATION_WITHPC_USART_H_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f446xx.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
typedef struct {
  uint8_t id;
  int16_t temperature;
  float pressure;
} SensorData;
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* External variables --------------------------------------------------------*/
void InitMainUART(void);
void sendSensorData(SensorData* data);
void TestWriteMessageFunction(void);
void writeData(const char* _data, uint8_t _size);
#endif /* TASKS_COMMUNICATION_WITHPC_USART_H_ */
