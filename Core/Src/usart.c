/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "uart_rings.h"

uart_dma_tx_ring_t uart1_tx_ring = {
	.head = 0,
	.tail = 0,
	.length = USART_TX_RING_SIZE,
	.available_size = USART_TX_RING_SIZE,
	.dma_last_size = 0,
	.dma_busy = 0
};

uart_dma_rx_ring_t uart1_rx_ring = {
	.head = 0,
	.tail = 0,
	.length = USART_RX_RING_SIZE,
	.available_size = USART_RX_RING_SIZE,
	.dma_last_size = 0,
	.dma_busy = 0,
};

uart_instance_t uart_instances[] = {
    { &huart1, &uart1_tx_ring, &uart1_rx_ring },
};

HAL_StatusTypeDef uart_start_queued_tx_dma_transmit(UART_HandleTypeDef* huart);
HAL_StatusTypeDef uart_start_rx_dma_receive(UART_HandleTypeDef* huart);


/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// Retrieve the TX ring buffer associated with a UART instance
uart_dma_tx_ring_t* uart_get_tx_ring(UART_HandleTypeDef* huart)
{
    for (size_t i = 0; i < sizeof(uart_instances)/sizeof(uart_instances[0]); i++) {
        if (uart_instances[i].huart == huart)
            return uart_instances[i].tx_ring;
    }
    return NULL;
}

// Calculate the number of bytes to transmit in the next DMA operation
size_t uart_get_size_to_transmit_per_dma_operation(uart_dma_tx_ring_t* ring)
{
	if (ring->head == ring->tail && ring->available_size < ring->length)
		return ring->length - ring->head;
	else if (ring->head <= ring->tail)
		return ring->tail - ring->head;
	else
		return ring->length - ring->head;
}

// Queue data for transmission via DMA
uart_dma_enqueue_tx_result_t uart_tx_queue_dma_transmit(UART_HandleTypeDef* huart, uint8_t* data, uint16_t size)
{
	uart_dma_tx_ring_t* ring = uart_get_tx_ring(huart);
	if (!ring) return UART_TX_RESULT_FAILURE;

	if (size == 0)
		return UART_TX_RESULT_FAILURE;
	if (uart_get_free_tx_ring_space(ring) < size)
		return UART_TX_RESULT_FAILURE;

	if (uart_append_tx_ring(ring, data, size) != 0)
		return UART_TX_RESULT_FAILURE;

	// Start DMA immediately if not already busy
	if (ring->dma_busy == 0)
		uart_start_queued_tx_dma_transmit(huart);

	return UART_TX_RESULT_QUEUED;
}

// Start DMA transmission of queued TX data
HAL_StatusTypeDef uart_start_queued_tx_dma_transmit(UART_HandleTypeDef* huart)
{
	uart_dma_tx_ring_t* ring = uart_get_tx_ring(huart);
	if (!ring) return HAL_ERROR;

	int size_to_transmit = uart_get_size_to_transmit_per_dma_operation(ring);

	if (size_to_transmit == 0) {
		ring->dma_busy = 0;
	    return HAL_ERROR;
	}

	ring->dma_busy = 1;
	ring->dma_last_size = size_to_transmit;
	HAL_StatusTypeDef hal_result = HAL_UART_Transmit_DMA(huart, &(ring->data[ring->head]), size_to_transmit);
	if (hal_result != HAL_OK)
	{
		ring->dma_busy = 0;
		return HAL_ERROR;
	}

	return HAL_OK;
}

// Callback invoked when DMA TX transfer completes
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_dma_tx_ring_t* ring = uart_get_tx_ring(huart);
    int size_to_send_completed = ring->dma_last_size;
    uart_free_tx_ring_space(ring, size_to_send_completed);
    int size_to_transmit_pending = uart_get_size_to_transmit_per_dma_operation(ring);

    // Continue transmitting remaining data if any
    if (size_to_transmit_pending != 0) {
    	uart_start_queued_tx_dma_transmit(huart);
    } else {
        ring->dma_busy = 0;
    }
}

// Retrieve the RX ring buffer associated with a UART instance
uart_dma_rx_ring_t* uart_get_rx_ring(UART_HandleTypeDef* huart)
{
    for (size_t i = 0; i < sizeof(uart_instances)/sizeof(uart_instances[0]); i++) {
        if (uart_instances[i].huart == huart)
            return uart_instances[i].rx_ring;
    }
    return NULL;
}

// Calculate the number of bytes that can be received in the next DMA operation
size_t uart_get_size_to_receive_per_dma_operation(uart_dma_rx_ring_t* ring)
{
	if (ring->head == ring->tail && ring->available_size != 0)
		return ring->length - ring->tail;
	if (ring->head < ring->tail)
		return ring->length - ring->tail;
	else
		return ring->head - ring->tail;
}

// Read pending data from RX DMA buffer into user buffer
size_t uart_rx_dma_get_pending_data(UART_HandleTypeDef* huart, uint8_t* destination, size_t max_length)
{
	uart_dma_rx_ring_t* ring = uart_get_rx_ring(huart);
	size_t pending_data_size = uart_get_pending_rx_ring_space(ring);
	if (pending_data_size == 0)
		return 0;

	int bytes_copied = uart_read_rx_ring(ring, destination, max_length);
	if (ring->dma_busy == 0)
		uart_start_rx_dma_receive(huart);

	return bytes_copied;
}

// Start DMA reception into RX ring buffer
HAL_StatusTypeDef uart_start_rx_dma_receive(UART_HandleTypeDef* huart)
{
	uart_dma_rx_ring_t* ring = uart_get_rx_ring(huart);
	if (!ring) return HAL_ERROR;

	int size_to_receive = uart_get_size_to_receive_per_dma_operation(ring);

	if (uart_get_free_rx_ring_space(ring) == 0) {
		ring->dma_busy = 0;
	    return HAL_ERROR;
	}

	ring->dma_busy = 1;
	ring->dma_last_size = size_to_receive;
	ring->dma_received_during_current_transfer = 0;
	HAL_StatusTypeDef hal_result = HAL_UARTEx_ReceiveToIdle_DMA(huart, &(ring->data[ring->tail]), size_to_receive);
	if (hal_result != HAL_OK)
	{
		ring->dma_busy = 0;
		return HAL_ERROR;
	}

	return HAL_OK;
}

// Callback invoked on RX DMA event (e.g., IDLE or partial reception)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size_to_receive_completed)
{
	uart_dma_rx_ring_t* ring = uart_get_rx_ring(huart);
	size_t new_bytes_received = size_to_receive_completed - ring->dma_received_during_current_transfer;
    uart_alloc_rx_ring_space(ring, new_bytes_received);
    int size_to_receive_pending = uart_get_size_to_receive_per_dma_operation(ring);
	ring->dma_received_during_current_transfer += new_bytes_received;

	// Check if DMA is still active
	HAL_DMA_StateTypeDef state = HAL_DMA_GetState(huart->hdmarx);
	int is_dma_still_active = (state == HAL_DMA_STATE_BUSY);

    // Start next DMA receive if pending and previous transfer finished
    if (size_to_receive_pending != 0 && !is_dma_still_active) {
    	uart_start_rx_dma_receive(huart);
    } else if (!is_dma_still_active && size_to_receive_pending) {
        ring->dma_busy = 0;
    }
}


/* USER CODE END 1 */
