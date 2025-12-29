/*
 * dma_ring_buffer.c
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#include <dma_ring_buffer.h>

/**
 * @brief Calculate the number of bytes to produced in the next DMA operation.
 * @param rb Pointer to ring buffer.
 * @return Number of bytes to transmit.
 */
size_t get_size_to_produce_per_dma_operation(ring_buffer_t* rb)
{
	if (rb->head == rb->tail && rb->available_size < rb->length)
		return rb->length - rb->head;
	else if (rb->head <= rb->tail)
		return rb->tail - rb->head;
	else
		return rb->length - rb->head;
}

/**
 * @brief Calculate the number of bytes that can be consumed in the next DMA operation.
 * @param rb Pointer to ring buffer.
 * @return Number of bytes available for DMA reception.
 */
size_t get_size_to_consume_per_dma_operation(ring_buffer_t* rb)
{
	if (rb->head == rb->tail && rb->available_size != 0)
		return rb->length - rb->tail;
	if (rb->head < rb->tail)
		return rb->length - rb->tail;
	else
		return rb->head - rb->tail;
}
