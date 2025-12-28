/*
 * uart_types.c
 *
 *  Creation date: 27 December 2025
 *      Author: ASMcoder
 */
#include <string.h>
#include <stdint.h>
#include <uart_rings.h>

// Returns free space in USART TX ring buffer
size_t uart_get_free_tx_ring_space(uart_dma_tx_ring_t* ring)
{
    return ring->available_size;
}

// Returns free space in USART RX ring buffer
size_t uart_get_free_rx_ring_space(uart_dma_rx_ring_t* ring)
{
	return ring->available_size;
}

// Returns number of bytes pending in RX ring buffer (ready to read)
size_t uart_get_pending_rx_ring_space(uart_dma_rx_ring_t* ring)
{
	return ring->length - ring->available_size;
}

// Append data to TX ring buffer (handles wrap-around)
int uart_append_tx_ring(uart_dma_tx_ring_t* ring, uint8_t* source, size_t size)
{
	if (size == 0)
		return -1;
	if (size > uart_get_free_tx_ring_space(ring))
		return -1;

	size_t size_till_ring_wrap = ring->length - ring->tail;
	size_t size_to_append_after_tail = size >= size_till_ring_wrap ? size_till_ring_wrap : size;

	// Copy data until the end of buffer, then wrap around if needed
	memcpy(&(ring->data[ring->tail]), source, size_to_append_after_tail);
	memcpy(&(ring->data[0]), &(source[size_to_append_after_tail]), size - size_to_append_after_tail);

	uart_alloc_tx_ring_space(ring, size);
	return 0;
}

// Move TX tail forward and reduce available space
void uart_alloc_tx_ring_space(uart_dma_tx_ring_t* ring, size_t size)
{
	ring->tail = (ring->tail + size) % ring->length;
	ring->available_size -= size;
}

// Move TX head forward and increase available space
void uart_free_tx_ring_space(uart_dma_tx_ring_t* ring, size_t size)
{
	ring->head = (ring->head + size) % ring->length;
	ring->available_size += size;
}

// Read data from RX ring buffer into user buffer (handles wrap-around)
int uart_read_rx_ring(uart_dma_rx_ring_t* ring, uint8_t* destination, size_t max_length)
{
    size_t pending_size = uart_get_pending_rx_ring_space(ring);

    if (max_length == 0 || pending_size == 0)
        return 0;
    if (max_length > pending_size) {
        max_length = pending_size;
    }

	size_t size_till_ring_wrap = ring->length - ring->head;
	size_t size_to_copy_till_ring_wrap = max_length > size_till_ring_wrap ? size_till_ring_wrap : max_length;
	size_t size_to_copy_after_ring_wrap = max_length - size_to_copy_till_ring_wrap;

	// Copy data until end of buffer, then wrap around if necessary
	memcpy(destination, &(ring->data[ring->head]), size_to_copy_till_ring_wrap);
	memcpy(&destination[size_to_copy_till_ring_wrap], &ring->data[0], size_to_copy_after_ring_wrap);

	uart_free_rx_ring_space(ring, max_length);
	return max_length;
}

// Move RX tail forward and reduce available space
void uart_alloc_rx_ring_space(uart_dma_rx_ring_t* ring, size_t size)
{
	ring->tail = (ring->tail + size) % ring->length;
	ring->available_size -= size;
}

// Move RX head forward and increase available space
void uart_free_rx_ring_space(uart_dma_rx_ring_t* ring, size_t size)
{
	ring->head = (ring->head + size) % ring->length;
	ring->available_size += size;
}

