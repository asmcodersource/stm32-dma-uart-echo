/*
 * dma_ring_buffer.h
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#ifndef __DMA_RING_BUFFER_H__
#define __DMA_RING_BUFFER_H__

#include <ring_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	ring_buffer_t* ring_buffer;
    size_t dma_last_size;
    int dma_busy;
} dma_producer_ring_t;

typedef struct {
	ring_buffer_t* ring_buffer;
    size_t dma_last_size;
    size_t dma_received_during_current_transfer;
    int dma_busy;
} dma_consumer_ring_t;


/**
 * @brief Calculate the number of bytes to produced in the next DMA operation.
 * @param rb Pointer to ring buffer.
 * @return Number of bytes to transmit.
 */
size_t get_size_to_produce_per_dma_operation(ring_buffer_t* rb);

/**
 * @brief Calculate the number of bytes that can be consumed in the next DMA operation.
 * @param rb Pointer to ring buffer.
 * @return Number of bytes available for DMA reception.
 */
size_t get_size_to_consume_per_dma_operation(ring_buffer_t* rb);

#ifdef __cplusplus
}
#endif

#endif
