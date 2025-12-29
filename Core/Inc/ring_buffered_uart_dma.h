/*
 * ring_buffered_uart_dma.h
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#ifndef __RING_BUFFERED_UART_DMA_H__
#define __RING_BUFFERED_UART_DMA_H__

#include <ring_buffer.h>
#include <dma_ring_buffer.h>
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USART_TX_RING_SIZE 1024
#define USART_RX_RING_SIZE 1024


typedef struct {
    UART_HandleTypeDef* huart;
    dma_producer_ring_t* tx_ring;
    dma_consumer_ring_t* rx_ring;
} uart_dma_buffered_instance_t;

typedef enum {
    UART_TX_RESULT_FAILURE = -1,
    UART_TX_RESULT_QUEUED = 0,
} uart_dma_enqueue_tx_result_t;

typedef enum {
    UART_RX_RESULT_FAILURE = -1,
    UART_RX_RESULT_QUEUED = 0,
} uart_dma_enqueue_rx_result_t;


extern UART_HandleTypeDef huart1;

/**
 * @brief Retrieve the TX ring buffer associated with a UART instance.
 * @param huart Pointer to UART handle.
 * @return Pointer to the TX ring buffer, or NULL if not found.
 */
dma_producer_ring_t* uart_get_tx_ring(UART_HandleTypeDef* huart);

/**
 * @brief Retrieve the RX ring buffer associated with a UART instance.
 * @param huart Pointer to UART handle.
 * @return Pointer to the RX ring buffer, or NULL if not found.
 */
dma_consumer_ring_t* uart_get_rx_ring(UART_HandleTypeDef* huart);


/**
 * @brief Queue data for transmission via DMA.
 *
 * Copies the data into the TX ring buffer and starts DMA if idle.
 *
 * @param huart Pointer to UART handle.
 * @param data Pointer to source data buffer.
 * @param size Number of bytes to enqueue.
 * @return UART_TX_RESULT_QUEUED if successfully queued, UART_TX_RESULT_FAILURE otherwise.
 */
uart_dma_enqueue_tx_result_t uart_tx_queue_dma_transmit(UART_HandleTypeDef* huart, uint8_t* data, uint16_t size);

/**
 * @brief Start DMA transmission of queued TX data.
 *
 * Initiates DMA for remaining data in the TX ring buffer.
 *
 * @param huart Pointer to UART handle.
 * @return HAL_OK if DMA started successfully, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef uart_start_queued_tx_dma_transmit(UART_HandleTypeDef* huart);

/**
 * @brief Read pending data from RX DMA buffer into user buffer.
 *
 * Copies data from the RX ring buffer and starts DMA if idle.
 *
 * @param huart Pointer to UART handle.
 * @param destination Pointer to destination buffer.
 * @param max_length Maximum number of bytes to read.
 * @return Number of bytes actually copied.
 */
size_t uart_rx_dma_get_pending_data(UART_HandleTypeDef* huart, uint8_t* destination, size_t max_length);

/**
 * @brief Start DMA reception into RX ring buffer.
 *
 * Initiates a DMA transfer to fill the RX ring buffer.
 *
 * @param huart Pointer to UART handle.
 * @return HAL_OK if DMA started successfully, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef uart_start_rx_dma_receive(UART_HandleTypeDef* huart);


#ifdef __cplusplus
}
#endif

#endif
