/*
 * ring_buffer.h
 *
 *  Created on: 29 December 2025.
 *      Author: ASMcoder
 */

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Ring buffer descriptor (single-producer / single-consumer).
 *
 * Head is advanced by producer (writer).
 * Tail is advanced by consumer (reader).
 *
 * Buffer is considered full when free space equals zero.
 */
typedef struct {
    uint8_t* data;          /**< Pointer to raw buffer memory */
    size_t   head;          /**< Write index (producer position) */
    size_t   tail;          /**< Read index (consumer position) */
    size_t   length;        /**< Total buffer size in bytes */
    size_t   available_size;/**< Cached number of free bytes */
} ring_buffer_t;

/**
 * @brief Get number of free bytes available for writing.
 * @param rb Pointer to ring buffer instance.
 * @return Number of free bytes.
 */
size_t ring_buffer_get_free_size(const ring_buffer_t* rb);

/**
 * @brief Get number of bytes currently stored in buffer.
 * @param rb Pointer to ring buffer instance.
 * @return Number of used bytes.
 */
size_t ring_buffer_get_used_size(const ring_buffer_t* rb);

/**
 * @brief Write data into ring buffer.
 *
 * Copies @p len bytes from source buffer and advances write pointer.
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
);

/**
 * @brief Read data from ring buffer.
 *
 * Copies up to @p len bytes into destination buffer
 * and advances read pointer.
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
);

/**
 * @brief Advance write pointer without copying data.
 *
 * Used when data is produced directly by hardware (e.g. DMA).
 *
 * @param rb Pointer to ring buffer instance.
 * @param len Number of bytes produced.
 */
void ring_buffer_produce(
    ring_buffer_t* rb,
    size_t len
);

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
);

/**
 * @brief Advance read pointer and update free space.
 *
 * Internal helper: marks data as consumed.
 *
 * @param rb Pointer to ring buffer instance.
 * @param size Number of bytes to consume.
 */
void ring_buffer_alloc_space(
    ring_buffer_t* rb,
    size_t size
);

/**
 * @brief Advance write pointer and update free space.
 *
 * Internal helper: marks data as produced.
 *
 * @param rb Pointer to ring buffer instance.
 * @param size Number of bytes produced.
 */
void ring_buffer_free_space(
    ring_buffer_t* rb,
    size_t size
);


#ifdef __cplusplus
}
#endif

#endif /* __RING_BUFFER_H__ */
