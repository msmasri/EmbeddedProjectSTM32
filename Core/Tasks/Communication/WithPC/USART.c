/*******************************************************************************
 * @file    USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "USART.h"

#include <stdio.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//           f_clk = 8MHz = f_clk_APB2			      45MHz
// ---------------------------------------- = --------------- =  =
//            8*(2-OVER8)*USARTDIV              16*115200
// 45MHz -> 24,4140625, fraction =  6,625
#define BAUDRATE 0x186
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static char message[] = "Hello world!\n";
static char MessageBuffer[100];

/* Private function prototypes -----------------------------------------------*/
inline static void InitUARTGPIO(void);
inline static void InitUART(void);
inline static void InitDMA_Reciever(void);
inline static void InitDMA_Transfer(void);
/* External variables
 * --------------------------------------------------------*/
/*
 * @note:USART2_TX-> PA2, USART2_RX-> PA3, AF7, APB
 * */
inline static void InitUARTGPIO(void) {
  /* Enable port A*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* select alternate function mode */
  GPIOA->MODER &= ~(GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
  GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);

  /* Output type Output open-drain */
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);

  /* Output speed medium*/
  GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2_0 | GPIO_OSPEEDER_OSPEEDR3_0);
  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDER_OSPEEDR3_1);

  /* Pull-up/Down UP*/
  GPIOA->PUPDR |= (GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0);

  /* alternative function 7*/
  GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_3 | GPIO_AFRL_AFSEL3_3);
}
// 8 data, 1 stop, 2600 baud rate, odd
inline static void InitUART(void) {
  /* enable clock */
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  /* Enable parity control */
  USART2->CR1 |= USART_CR1_PCE;

  /* Bit 2 PS: Parity selection, odd */
  USART2->CR1 |= USART_CR1_PS;

  /* OVER8: Oversampling mode by 16 */
  USART2->CR1 &= ~USART_CR1_OVER8;

  /* word length 0: 1 Start bit, 2 Data bits, n Stop bit*/
  USART2->CR1 |= USART_CR1_M;

  /* one stop bit */
  USART2->CR2 &= ~USART_CR2_STOP;

  /* ONEBIT: One sample bit method enable */
  USART2->CR3 |= USART_CR3_ONEBIT;

  // select DMA for transmitter and receiver
  USART2->CR3 |= (USART_CR3_DMAR | USART_CR3_DMAT);

  /* select Baud rate */
  USART2->BRR = BAUDRATE;

  /* TE: Transmitter, RE: Receiver and USART enable */
  // USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
  USART2->CR1 |= (USART_CR1_TE | USART_CR1_UE);
}

/* stream 5 for USART2_Rx, channel 4
 * stream 6 for USART2_Tx, channel 4
 * DMA1
 */
inline static void InitDMA_Reciever(void) {
  /******** ######################################### ***********/
  /******** configure stream 5 for USART2_Rx, channel 4 *********/

  /* enable DMA2 */
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  /* select channel 4 */
  DMA1_Stream5->CR &= ~DMA_SxCR_CHSEL;
  DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2;

  /* size of the data  8-bits*/
  DMA1_Stream5->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);

  /* increment memory address */
  DMA1_Stream5->CR |= DMA_SxCR_MINC;
  /* fixed peripheral address */
  DMA1_Stream5->CR &= ~DMA_SxCR_PINC;

  /* direction peripheral to memory */
  DMA1_Stream5->CR &= ~DMA_SxCR_DIR;

  /* address of the peripheral */
  // DMA1_Stream5->PAR = (uint32_t)(&USART2->DR);

  /* address of the memory, where the data should be stored from data
    buffer*/
  // DMA1_Stream5->M0AR = (uint32_t)(&MessageBuffer);
}

inline static void InitDMA_Transfer(void) {
  /****** ########################################### ***********/
  /****** configure stream 6 for USART2_Tx, channel 4 ***********/

  /* enable DMA2 */
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  /* disable stream 7 */
  DMA1_Stream6->CR &= ~DMA_SxCR_EN;

  /* select channel 4 */
  DMA1_Stream6->CR &= ~DMA_SxCR_CHSEL;
  DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;

  /* size of the data memory and peripheral 8-bits*/
  DMA1_Stream6->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);

  /* increment memory address */
  DMA1_Stream6->CR |= DMA_SxCR_MINC;
  /* fixed peripheral address */
  DMA1_Stream6->CR &= ~DMA_SxCR_PINC;

  /* direction memory to peripheral*/
  DMA1_Stream6->CR &= ~DMA_SxCR_DIR;
  DMA1_Stream6->CR |= DMA_SxCR_DIR_0;

  /* Enable DMA transfer complete interrupt */
  DMA1_Stream6->CR |= DMA_SxCR_TCIE;

  /* address of the peripheral */
  DMA1_Stream6->PAR = (uint32_t)(&USART2->DR);

  NVIC_SetPriority(DMA1_Stream6_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void InitMainUART(void) {
  InitUARTGPIO();
  InitUART();
  InitDMA_Transfer();
}

void writeData(const char* _data, uint8_t _size) {
  /* Wait until DMA1_stream6 is disabled */
  while (DMA_SxCR_EN == (DMA_SxCR_EN & DMA1_Stream6->CR)) {
  }

  /* Set memory address */
  DMA1_Stream6->M0AR = (uint32_t)_data;

  /* Set number of data items to be transfered */
  DMA1_Stream6->NDTR = (uint32_t)_size;

  /* Enable stream 6 in DMA1 */
  DMA1_Stream6->CR |= DMA_SxCR_EN;
}

void DMA1_Stream6_IRQHandler(void) {
  // /* Clear all interrupt flags */
  DMA1->HIFCR = (DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF6 |
                 DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTCIF6);
}
void readData(void) {}

void TestWriteMessageFunction(void) {
  uint8_t idx = 0;
  while (message[idx] != '\0') {
    if ((USART2->SR & USART_SR_TXE) == USART_SR_TXE) {
      USART2->DR = (uint8_t)message[idx++];
    }
  }
}