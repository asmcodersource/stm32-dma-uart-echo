/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "uart_rings.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
typedef struct {
    UART_HandleTypeDef* huart;
    uart_dma_tx_ring_t* tx_ring;
    uart_dma_rx_ring_t* rx_ring;
} uart_instance_t;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
uart_dma_enqueue_tx_result_t uart_tx_queue_dma_transmit(UART_HandleTypeDef* huart, uint8_t* data, uint16_t size);
size_t uart_rx_dma_get_pending_data(UART_HandleTypeDef* huart, uint8_t* destination, size_t max_length);
HAL_StatusTypeDef uart_start_rx_dma_receive(UART_HandleTypeDef* huart);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

