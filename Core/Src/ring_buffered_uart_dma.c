/*
 * ring_buffered_uart_dma.c
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#include <ring_buffered_uart_dma.h>
#include <ring_buffer.h>
#include <dma_ring_buffer.h>
#include <string.h>
#include <stdint.h>

uint8_t uart1_tx_ring_buffer_data[USART_TX_RING_SIZE];
uint8_t uart1_rx_ring_buffer_data[USART_RX_RING_SIZE];

ring_buffer_t uart1_tx_ring_buffer = {
	.data = uart1_tx_ring_buffer_data,
	.available_size = USART_TX_RING_SIZE,
	.length = USART_TX_RING_SIZE,
	.head = 0,
	.tail = 0,
};

ring_buffer_t uart1_rx_ring_buffer = {
	.data = uart1_rx_ring_buffer_data,
	.available_size = USART_RX_RING_SIZE,
	.length = USART_RX_RING_SIZE,
	.head = 0,
	.tail = 0,
};

dma_producer_ring_t uart1_tx_ring = {
	.ring_buffer = &uart1_tx_ring_buffer,
	.dma_last_size = 0,
	.dma_busy = 0
};


dma_consumer_ring_t uart1_rx_ring = {
	.ring_buffer = &uart1_rx_ring_buffer,
	.dma_last_size = 0,
	.dma_busy = 0,
};

uart_dma_buffered_instance_t uart_instances[] = {
    { &huart1, &uart1_tx_ring, &uart1_rx_ring },
};


/**
 * @brief Retrieve the TX ring buffer associated with a UART instance.
 * @param huart Pointer to UART handle.
 * @return Pointer to the TX ring buffer, or NULL if not found.
 */
dma_producer_ring_t* uart_get_tx_ring(UART_HandleTypeDef* huart)
{
    for (size_t i = 0; i < sizeof(uart_instances)/sizeof(uart_instances[0]); i++) {
        if (uart_instances[i].huart == huart)
            return uart_instances[i].tx_ring;
    }
    return NULL;
}

/**
 * @brief Retrieve the RX ring buffer associated with a UART instance.
 * @param huart Pointer to UART handle.
 * @return Pointer to the RX ring buffer, or NULL if not found.
 */
dma_consumer_ring_t* uart_get_rx_ring(UART_HandleTypeDef* huart)
{
    for (size_t i = 0; i < sizeof(uart_instances)/sizeof(uart_instances[0]); i++) {
        if (uart_instances[i].huart == huart)
            return uart_instances[i].rx_ring;
    }
    return NULL;
}

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
uart_dma_enqueue_tx_result_t uart_tx_queue_dma_transmit(UART_HandleTypeDef* huart, uint8_t* data, uint16_t size)
{
	dma_producer_ring_t* r = uart_get_tx_ring(huart);
	if (!r) return UART_TX_RESULT_FAILURE;

	ring_buffer_t* rb = r->ring_buffer;

	if (size == 0)
		return UART_TX_RESULT_FAILURE;
	if (ring_buffer_get_free_size(rb) < size)
		return UART_TX_RESULT_FAILURE;

	if (ring_buffer_write(rb, data, size) != 0)
		return UART_TX_RESULT_FAILURE;

	// Start DMA immediately if not already busy
	if (r->dma_busy == 0)
		uart_start_queued_tx_dma_transmit(huart);

	return UART_TX_RESULT_QUEUED;
}

/**
 * @brief Start DMA transmission of queued TX data.
 *
 * Initiates DMA for remaining data in the TX ring buffer.
 *
 * @param huart Pointer to UART handle.
 * @return HAL_OK if DMA started successfully, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef uart_start_queued_tx_dma_transmit(UART_HandleTypeDef* huart)
{
	dma_producer_ring_t* r = uart_get_tx_ring(huart);
	if (!r) return HAL_ERROR;

	ring_buffer_t* rb = r->ring_buffer;

	int size_to_transmit = get_size_to_produce_per_dma_operation(rb);

	if (size_to_transmit == 0) {
		r->dma_busy = 0;
	    return HAL_ERROR;
	}

	r->dma_busy = 1;
	r->dma_last_size = size_to_transmit;
	HAL_StatusTypeDef hal_result = HAL_UART_Transmit_DMA(huart, rb->data + rb->head, size_to_transmit);
	if (hal_result != HAL_OK)
	{
		r->dma_busy = 0;
		return HAL_ERROR;
	}

	return HAL_OK;
}

// Callback invoked when DMA TX transfer completes
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	dma_producer_ring_t* r = uart_get_tx_ring(huart);
	ring_buffer_t* rb = r->ring_buffer;

    int size_to_send_completed = r->dma_last_size;
    ring_buffer_produce(rb, size_to_send_completed);
    int size_to_transmit_pending = get_size_to_produce_per_dma_operation(rb);

    // Continue transmitting remaining data if any
    if (size_to_transmit_pending != 0) {
    	uart_start_queued_tx_dma_transmit(huart);
    } else {
        r->dma_busy = 0;
    }
}


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
size_t uart_rx_dma_get_pending_data(UART_HandleTypeDef* huart, uint8_t* destination, size_t max_length)
{
	dma_consumer_ring_t* r = uart_get_rx_ring(huart);
	ring_buffer_t* rb = r->ring_buffer;

	size_t pending_data_size = ring_buffer_get_used_size(rb);
	if (pending_data_size == 0)
		return 0;

	int bytes_copied = ring_buffer_read(rb, destination, max_length);
	if (r->dma_busy == 0)
		uart_start_rx_dma_receive(huart);

	return bytes_copied;
}

/**
 * @brief Start DMA reception into RX ring buffer.
 *
 * Initiates a DMA transfer to fill the RX ring buffer.
 *
 * @param huart Pointer to UART handle.
 * @return HAL_OK if DMA started successfully, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef uart_start_rx_dma_receive(UART_HandleTypeDef* huart)
{
	dma_consumer_ring_t* r = uart_get_rx_ring(huart);
	if (!r) return HAL_ERROR;

	ring_buffer_t* rb = r->ring_buffer;
	int size_to_receive = get_size_to_consume_per_dma_operation(rb);

	if (ring_buffer_get_free_size(rb) == 0) {
		r->dma_busy = 0;
	    return HAL_ERROR;
	}

	r->dma_busy = 1;
	r->dma_last_size = size_to_receive;
	r->dma_received_during_current_transfer = 0;
	HAL_StatusTypeDef hal_result = HAL_UARTEx_ReceiveToIdle_DMA(huart, rb->data + rb->tail, size_to_receive);
	if (hal_result != HAL_OK)
	{
		r->dma_busy = 0;
		return HAL_ERROR;
	}

	return HAL_OK;
}

// Callback invoked on RX DMA event (e.g., IDLE or partial reception)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size_to_receive_completed)
{
	dma_consumer_ring_t* r = uart_get_rx_ring(huart);
	ring_buffer_t* rb = r->ring_buffer;
	size_t new_bytes_received = size_to_receive_completed - r->dma_received_during_current_transfer;
    ring_buffer_consume(rb, new_bytes_received);
    int size_to_receive_pending = get_size_to_consume_per_dma_operation(rb);
	r->dma_received_during_current_transfer += new_bytes_received;

	// Check if DMA is still active
	HAL_DMA_StateTypeDef state = HAL_DMA_GetState(huart->hdmarx);
	int is_dma_still_active = (state == HAL_DMA_STATE_BUSY);

    // Start next DMA receive if pending and previous transfer finished
    if (size_to_receive_pending != 0 && !is_dma_still_active) {
    	uart_start_rx_dma_receive(huart);
    } else if (!is_dma_still_active && size_to_receive_pending) {
        r->dma_busy = 0;
    }
}
