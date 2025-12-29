/*
 * ring_buffer.c
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#include "ring_buffer.h"
#include <string.h>


/**
 * @brief Get number of free bytes available for writing.
 * @param rb Pointer to ring buffer instance.
 * @return Number of free bytes.
 */
size_t ring_buffer_get_free_size(const ring_buffer_t* rb)
{
	return rb->available_size;
}

/**
 * @brief Get number of bytes currently stored in buffer.
 * @param rb Pointer to ring buffer instance.
 * @return Number of used bytes.
 */
size_t ring_buffer_get_used_size(const ring_buffer_t* rb)
{
	return rb->length - rb->available_size;
}

/**
 * @brief Write data into ring buffer and advance write pointer.
 *
 * Copies up to @p len bytes from source into buffer.
 * Actual number of bytes written may be smaller if buffer is full.
 *
 * @param rb Pointer to ring buffer instance.
 * @param src Source data buffer.
 * @param len Number of bytes to write.
 * @return Number of bytes actually written.
 */
int ring_buffer_write(
    ring_buffer_t* rb,
    const uint8_t* src,
    int len
)
{
	if (len == 0)
		return 0;
	if (len > ring_buffer_get_free_size(rb))
		return -1;

	int size_till_ring_wrap = rb->length - rb->tail;
	int size_to_append_after_tail = len >= size_till_ring_wrap ? size_till_ring_wrap : len;

	// Copy data until the end of buffer, then wrap around if needed
	memcpy(rb->data + rb->tail, src, size_to_append_after_tail);
	memcpy(rb->data, src + size_to_append_after_tail, len - size_to_append_after_tail);

	ring_buffer_alloc_space(rb, len);
	return 0;
}

/**
 * @brief Advance write pointer without copying data.
 *
 * Used when data is written directly by hardware (e.g. DMA).
 *
 * @param rb Pointer to ring buffer instance.
 * @param len Number of bytes produced.
 */
void ring_buffer_produce(
    ring_buffer_t* rb,
    size_t len
)
{
	ring_buffer_free_space(rb, len);
}

/**
 * @brief Read data from ring buffer and advance read pointer.
 *
 * Copies up to @p len bytes from buffer into destination.
 * Actual number of bytes read may be smaller if buffer is empty.
 *
 * @param rb Pointer to ring buffer instance.
 * @param dst Destination buffer.
 * @param len Number of bytes to read.
 * @return Number of bytes actually read.
 */
size_t ring_buffer_read(
    ring_buffer_t* rb,
    uint8_t* dst,
    size_t len
)
{
	size_t pending_size = ring_buffer_get_used_size(rb);

	if (len == 0 || pending_size == 0)
		return 0;
	if (len > pending_size) {
		len = pending_size;
	}

	size_t size_till_ring_wrap = rb->length - rb->head;
	size_t size_to_copy_till_ring_wrap = len > size_till_ring_wrap ? size_till_ring_wrap : len;

	// Copy data until end of buffer, then wrap around if necessary
	memcpy(dst, rb->data + rb->head, size_to_copy_till_ring_wrap);
	memcpy(dst + size_to_copy_till_ring_wrap, rb->data, len - size_to_copy_till_ring_wrap);

	ring_buffer_free_space(rb, len);
	return len;
}

/**
 * @brief Advance read pointer without copying data.
 *
 * Used when data is consumed without CPU copy.
 *
 * @param rb Pointer to ring buffer instance.
 * @param len Number of bytes consumed.
 */
void ring_buffer_consume(
    ring_buffer_t* rb,
    size_t len
)
{
	ring_buffer_alloc_space(rb, len);
}


/**
 * @brief Allocate space in ring buffer by advancing read pointer.
 *
 * Marks @p size bytes as consumed by the consumer.
 * No data is copied.
 *
 * Typically used after data has been read or processed.
 *
 * @param rb Pointer to ring buffer instance.
 * @param size Number of bytes to consume.
 */
void ring_buffer_alloc_space(ring_buffer_t* rb, size_t size)
{
    rb->tail = (rb->tail + size) % rb->length;
    rb->available_size -= size;
}

/**
 * @brief Free space in ring buffer by advancing write pointer.
 *
 * Marks @p size bytes as produced by the producer.
 * No data is copied.
 *
 * Typically used after data has been written directly
 * by hardware (e.g. DMA).
 *
 * @param rb Pointer to ring buffer instance.
 * @param size Number of bytes produced.
 */
void ring_buffer_free_space(ring_buffer_t* rb, size_t size)
{
    rb->head = (rb->head + size) % rb->length;
    rb->available_size += size;
}
